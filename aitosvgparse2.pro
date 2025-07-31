# Add all required Qt modules: widgets (includes core & gui) and svg
QT += widgets svg gui core svgwidgets xml

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000  # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# =======================================================
# Manual Linking for Poppler and Dependencies on Windows
# =======================================================

# Poppler include directories
INCLUDEPATH += \
    C:/Libraries/poppler-qtmingw/include/poppler/qt6 \
    C:/Libraries/poppler-qtmingw/include

# Poppler library directories
LIBS += -LC:/Libraries/poppler-qtmingw/lib

# Link with Poppler Qt6 and its core dependencies
LIBS += -lpoppler-qt6 -lpoppler

# --- FreeType & Zlib (required by Poppler) ---

# Include paths for dependencies
INCLUDEPATH += \
    C:/Libraries/freetype-qtmingw-dll/include/freetype2 \
    C:/Libraries/zlib-qtmingw-dll/include

# Library paths and libraries for dependencies
LIBS += -LC:/Libraries/freetype-qtmingw-dll/lib -lfreetype
LIBS += -LC:/Libraries/zlib-qtmingw-dll/lib -lz

# =======================================================

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
