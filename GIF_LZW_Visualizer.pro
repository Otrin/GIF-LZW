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
    animationThread.cpp

HEADERS  += mainWindow.h \
    animationThread.h

FORMS    += mainWindow.ui
