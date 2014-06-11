#-------------------------------------------------
#
# Project created by QtCreator 2014-05-28T09:01:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GIF_LZW_Visualizer
TEMPLATE = app


SOURCES += main.cpp\
        mainWindow.cpp \
    animationThread.cpp \
    codelist.cpp \
    codeword.cpp \
    gif.cpp \
    lzw.cpp

HEADERS  += mainWindow.h \
    animationThread.h \
    codelist.h \
    codeword.h \
    gif.h \
    lzw.h

FORMS    += mainWindow.ui
