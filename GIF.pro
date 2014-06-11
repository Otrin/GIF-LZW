#-------------------------------------------------
#
# Project created by QtCreator 2014-06-09T16:51:18
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = GIF
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    lzw.cpp \
    gif.cpp \
    codelist.cpp \
    codeword.cpp

HEADERS += \
    lzw.h \
    gif.h \
    codelist.h \
    codeword.h
