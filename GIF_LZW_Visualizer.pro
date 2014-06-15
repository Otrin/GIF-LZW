#-------------------------------------------------
#
# Project created by QtCreator 2014-05-28T09:01:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GIF_LZW_Visualizer
TEMPLATE = app


SOURCES += src/gui/main.cpp\
        src/gui/mainWindow.cpp \
    src/gui/animationThread.cpp \
    src/lzw/codelist.cpp \
    src/lzw/codeword.cpp \
    src/lzw/gif.cpp \
    src/lzw/lzw.cpp \
    src/globaltablecomparison/globaltablecomparison.cpp \
    src/lzw/io.cpp \
    src/lzw/image.cpp \
    src/lzw/picture.cpp

HEADERS  += src/gui/mainWindow.h \
    src/gui/animationThread.h \
    src/lzw/codelist.h \
    src/lzw/codeword.h \
    src/lzw/gif.h \
    src/lzw/lzw.h \
    src/globaltablecomparison/globaltablecomparison.h \
    src/lzw/io.h \
    src/lzw/image.h \
    src/lzw/picture.h

FORMS    += src/gui/mainWindow.ui

TRANSLATIONS = lang/Translation_en.ts \
    lang/Translation_de.ts


CONFIG(release, debug|release) {
    DESTDIR = release
} else {
    DESTDIR = debug
}

win32 {
    copyfiles.commands += @call xcopy /Y ..\\GIF-LZW\\lang\\*.* $${DESTDIR}\\lang\\
}
unix{
    copyfiles.commands += cp -rf ../GIF-LZW/lang $${DESTDIR}
}

QMAKE_EXTRA_TARGETS += copyfiles
POST_TARGETDEPS += copyfiles
