INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += src/gui/main.cpp \
        src/gui/mainWindow.cpp \
        src/gui/animationThread.cpp \
    src/gui/aboutdialog.cpp \
    src/gui/instructiondialog.cpp

HEADERS += src/gui/mainWindow.h \
    src/gui/animationThread.h \
    src/gui/aboutdialog.h \
    src/gui/instructiondialog.h

FORMS += src/gui/mainWindow.ui \
    src/gui/aboutdialog.ui \
    src/gui/instructiondialog.ui

