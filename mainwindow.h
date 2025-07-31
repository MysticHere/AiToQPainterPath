#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainterPath>
#include <QTransform>

// Forward declarations
class QGraphicsScene;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_selectAiButton_clicked();
    void on_convertAndParseButton_clicked();
    void on_saveAsButton_clicked(); // Slot for the new save button

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QPainterPath m_lastGeneratedPath; // To store the path for saving

    // --- Core Workflow Functions ---
    bool convertAiToSvgWithImage(const QString &aiFilePath, const QString &svgFilePath);
    bool createVectorSvgFromImageSvg(const QString &imageSvgPath, const QString &vectorSvgPath);

    // --- Manual Parsing Functions ---
    void parseSvgFile(const QString &filePath);
    QPainterPath parsePathData(const QString &data);
    QTransform parseTransform(const QString &transformStr);

    // --- Save Helper Functions ---
    void savePathToSvg(const QString &filePath);
    void saveSceneToPng(const QString &filePath);
};
#endif // MAINWINDOW_H
