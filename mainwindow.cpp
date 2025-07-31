#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QGraphicsSvgItem>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QTransform>
#include <QDebug>
#include <QRegularExpression>
#include <QDir>
#include <QImage>
#include <QSvgRenderer>
#include <QSvgGenerator>
#include <QMessageBox>
#include <poppler-qt6.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(new QGraphicsScene(this))
{
    ui->setupUi(this);
    ui->graphicsView->setScene(scene);
    ui->saveAsButton->setEnabled(false); // Disable save button initially
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectAiButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open AI File", "", "Adobe Illustrator Files (*.ai)");
    if (!fileName.isEmpty()) {
        ui->inputLineEdit->setText(fileName);
    }
}

void MainWindow::on_convertAndParseButton_clicked()
{
    QString aiPath = ui->inputLineEdit->text();
    if (aiPath.isEmpty()) {
        QMessageBox::warning(this, "Input Missing", "Please select an AI file first.");
        return;
    }

    // Reset state
    scene->clear();
    m_lastGeneratedPath = QPainterPath();
    ui->saveAsButton->setEnabled(false);

    QString tempImageSvgPath = QDir::temp().filePath("1_image_render.svg");
    QString tempVectorSvgPath = QDir::temp().filePath("2_vector_path.svg");

    ui->statusbar->showMessage("Step 1/3: Converting AI to SVG...");
    if (!convertAiToSvgWithImage(aiPath, tempImageSvgPath)) {
        QMessageBox::critical(this, "Error", "Failed to convert AI file to SVG.");
        return;
    }

    QGraphicsSvgItem *imageItem = new QGraphicsSvgItem(tempImageSvgPath);
    if (imageItem->renderer()->isValid()) {
        scene->addItem(imageItem);
        ui->graphicsView->fitInView(imageItem, Qt::KeepAspectRatio);
    } else {
        delete imageItem;
    }

    ui->statusbar->showMessage("Step 2/3: Creating vector SVG from image...");
    if (!createVectorSvgFromImageSvg(tempImageSvgPath, tempVectorSvgPath)) {
        QMessageBox::critical(this, "Error", "Failed to create a clean vector SVG.");
        return;
    }

    ui->statusbar->showMessage("Step 3/3: Parsing final SVG to QPainterPath...");
    parseSvgFile(tempVectorSvgPath);
    ui->statusbar->showMessage("Workflow complete!", 3000);
}

void MainWindow::on_saveAsButton_clicked()
{
    QString savePath = QFileDialog::getSaveFileName(this, "Save As", "", "PNG Image (*.png);;SVG Vector Path (*.svg)");

    if (savePath.isEmpty()) {
        return;
    }

    if (savePath.endsWith(".svg", Qt::CaseInsensitive)) {
        savePathToSvg(savePath);
    } else if (savePath.endsWith(".png", Qt::CaseInsensitive)) {
        saveSceneToPng(savePath);
    } else {
        QMessageBox::warning(this, "Invalid Format", "Please choose a valid file format (.png or .svg).");
    }
}

// --- Core Workflow Functions ---

bool MainWindow::convertAiToSvgWithImage(const QString &aiFilePath, const QString &svgFilePath)
{
    std::unique_ptr<Poppler::Document> doc = Poppler::Document::load(aiFilePath);
    if (!doc || doc->isLocked()) return false;
    if (doc->numPages() == 0) return false;
    std::unique_ptr<Poppler::Page> page = doc->page(0);
    if (!page) return false;
    QImage renderedImage = page->renderToImage(300, 300);
    if (renderedImage.isNull()) return false;
    QSvgGenerator generator;
    generator.setFileName(svgFilePath);
    generator.setSize(renderedImage.size());
    generator.setViewBox(renderedImage.rect());
    QPainter painter;
    if (!painter.begin(&generator)) return false;
    painter.drawImage(0, 0, renderedImage);
    painter.end();
    return true;
}

// **** THIS FUNCTION IS NOW CORRECTED ****
bool MainWindow::createVectorSvgFromImageSvg(const QString &imageSvgPath, const QString &vectorSvgPath)
{
    // Step 1: Load the image SVG into a QGraphicsSvgItem to render it
    QGraphicsSvgItem svgItem(imageSvgPath);
    if (!svgItem.renderer()->isValid()) return false;

    // Step 2: Get the QPainterPath from the item's SHAPE, not its viewbox.
    // This is the key change that extracts the actual shape outlines.
    QPainterPath path = svgItem.shape();
    QRectF bounds = path.boundingRect();

    // Step 3: Write a NEW, clean SVG file containing only this accurate path
    QFile file(vectorSvgPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("svg");
    xml.writeAttribute("width", QString::number(bounds.width()));
    xml.writeAttribute("height", QString::number(bounds.height()));
    // Use the path's bounds for the viewBox for a tight fit
    xml.writeAttribute("viewBox", QString("%1 %2 %3 %4").arg(bounds.x()).arg(bounds.y()).arg(bounds.width()).arg(bounds.height()));

    xml.writeStartElement("path");

    // Convert the QPainterPath to the SVG 'd' attribute string format
    QString d;
    for (int i = 0; i < path.elementCount(); ++i) {
        const QPainterPath::Element &el = path.elementAt(i);
        if (el.isMoveTo()) d += QString("M %1 %2 ").arg(el.x).arg(el.y);
        else if (el.isLineTo()) d += QString("L %1 %2 ").arg(el.x).arg(el.y);
        else if (el.isCurveTo()) d += QString("C %1 %2, %3 %4, %5 %6 ").arg(path.elementAt(i+1).x).arg(path.elementAt(i+1).y).arg(path.elementAt(i+2).x).arg(path.elementAt(i+2).y).arg(el.x).arg(el.y);
    }
    // Note: We don't manually close with 'Z' as the path object already contains this info.
    xml.writeAttribute("d", d);
    xml.writeAttribute("fill", "none");
    xml.writeAttribute("stroke", "black");
    xml.writeAttribute("stroke-width", "2");

    xml.writeEndElement(); // path
    xml.writeEndElement(); // svg
    xml.writeEndDocument();
    file.close();
    return true;
}

// --- Manual Parser & Save Functions ---

void MainWindow::parseSvgFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QXmlStreamReader xml(&file);
    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "path") {
            QString d = xml.attributes().value("d").toString();
            QString transform = xml.attributes().value("transform").toString();
            QPainterPath p = parsePathData(d);
            QTransform t = parseTransform(transform);
            QPainterPath transformed = t.map(p);
            QGraphicsPathItem *item = new QGraphicsPathItem(transformed);
            item->setPen(QPen(Qt::black, 2));
            scene->addItem(item);
            m_lastGeneratedPath = transformed; // Store the path
            ui->saveAsButton->setEnabled(true); // Enable saving
        }
    }
    file.close();
}

void MainWindow::savePathToSvg(const QString &filePath)
{
    QRectF bounds = m_lastGeneratedPath.boundingRect();
    QSvgGenerator generator;
    generator.setFileName(filePath);
    generator.setSize(bounds.size().toSize());
    generator.setViewBox(bounds);
    QPainter painter;
    painter.begin(&generator);
    QPainterPath finalPath = m_lastGeneratedPath;
    finalPath.translate(-bounds.topLeft());
    painter.setBrush(Qt::black);
    painter.drawPath(finalPath);
    painter.end();
    QMessageBox::information(this, "Save Successful", "The vector path was saved to:\n" + filePath);
}

void MainWindow::saveSceneToPng(const QString &filePath)
{
    // Render the entire scene to an image
    QRectF bounds = scene->itemsBoundingRect();
    QImage image(bounds.size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    scene->render(&painter, QRectF(), bounds);
    painter.end();
    if (image.save(filePath, "PNG")) {
        QMessageBox::information(this, "Save Successful", "The view was saved to:\n" + filePath);
    } else {
        QMessageBox::critical(this, "Save Error", "Failed to save the image.");
    }
}

QTransform MainWindow::parseTransform(const QString &transformStr)
{
    QTransform t;
    if (transformStr.startsWith("matrix")) {
        QString inside = transformStr.mid(transformStr.indexOf('(') + 1);
        inside.chop(1);
        QStringList values = inside.split(QRegularExpression("[ ,]"), Qt::SkipEmptyParts);
        if (values.size() == 6) {
            bool ok;
            t = QTransform(values[0].toDouble(&ok), values[1].toDouble(&ok),
                           values[2].toDouble(&ok), values[3].toDouble(&ok),
                           values[4].toDouble(&ok), values[5].toDouble(&ok));
        }
    }
    return t;
}

QPainterPath MainWindow::parsePathData(const QString &data)
{
    QPainterPath path;
    QChar lastCmd = ' ';
    QPointF lastPoint(0, 0), startPoint(0, 0);
    QString str = data.trimmed();
    str.replace(",", " ");
    QStringList tokens = str.split(QRegularExpression("\\s+|(?=[A-Za-z])|(?<=[A-Za-z])"), Qt::SkipEmptyParts);
    int i = 0;
    while (i < tokens.size()) {
        QString token = tokens[i];
        double x, y, x1, y1, x2, y2;
        QChar cmd;
        if (token[0].isLetter()) {
            cmd = token[0];
            i++;
        } else {
            cmd = lastCmd;
        }
        switch (cmd.toLatin1()) {
        case 'M': case 'm': {
            do {
                x = tokens[i++].toDouble(); y = tokens[i++].toDouble();
                if (cmd == 'm') { x += lastPoint.x(); y += lastPoint.y(); }
                path.moveTo(x, y);
                lastPoint = QPointF(x, y); startPoint = lastPoint;
                cmd = (cmd == 'M') ? 'L' : 'l';
            } while (i < tokens.size() && !tokens[i][0].isLetter());
            break;
        }
        case 'L': case 'l': {
            do {
                x = tokens[i++].toDouble(); y = tokens[i++].toDouble();
                if (cmd == 'l') { x += lastPoint.x(); y += lastPoint.y(); }
                path.lineTo(x, y);
                lastPoint = QPointF(x, y);
            } while (i < tokens.size() && !tokens[i][0].isLetter());
            break;
        }
        case 'H': case 'h': {
            do {
                x = tokens[i++].toDouble();
                if (cmd == 'h') { x += lastPoint.x(); }
                path.lineTo(x, lastPoint.y());
                lastPoint.setX(x);
            } while (i < tokens.size() && !tokens[i][0].isLetter());
            break;
        }
        case 'V': case 'v': {
            do {
                y = tokens[i++].toDouble();
                if (cmd == 'v') { y += lastPoint.y(); }
                path.lineTo(lastPoint.x(), y);
                lastPoint.setY(y);
            } while (i < tokens.size() && !tokens[i][0].isLetter());
            break;
        }
        case 'C': case 'c': {
            do {
                x1 = tokens[i++].toDouble(); y1 = tokens[i++].toDouble();
                x2 = tokens[i++].toDouble(); y2 = tokens[i++].toDouble();
                x = tokens[i++].toDouble();  y = tokens[i++].toDouble();
                if (cmd == 'c') {
                    x1 += lastPoint.x(); y1 += lastPoint.y();
                    x2 += lastPoint.x(); y2 += lastPoint.y();
                    x += lastPoint.x();  y += lastPoint.y();
                }
                path.cubicTo(QPointF(x1, y1), QPointF(x2, y2), QPointF(x, y));
                lastPoint = QPointF(x, y);
            } while (i < tokens.size() && !tokens[i][0].isLetter());
            break;
        }
        case 'Z': case 'z': {
            path.closeSubpath();
            lastPoint = startPoint;
            break;
        }
        default:
            if (!token[0].isLetter()) { i++; }
        }
        lastCmd = cmd;
    }
    return path;
}
