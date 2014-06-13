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
    src/lzw/io.cpp

HEADERS  += src/gui/mainWindow.h \
    src/gui/animationThread.h \
    src/lzw/codelist.h \
    src/lzw/codeword.h \
    src/lzw/gif.h \
    src/lzw/lzw.h \
    src/globaltablecomparison/globaltablecomparison.h \
    src/lzw/io.h

FORMS    += src/gui/mainWindow.ui
