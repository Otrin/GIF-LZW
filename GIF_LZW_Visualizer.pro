#-------------------------------------------------
#
# Project created by QtCreator 2014-05-28T09:01:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GIF_LZW_Visualizer
TEMPLATE = app

CONFIG += c++11

include(src/gui/gui.pri)
include(src/lzw/lzw.pri)
include(src/compressor/compressor.pri)
include(src/globaltablecomparison/globaltablecomparison.pri)
include(src/visual/visual.pri)

SOURCES +=

HEADERS +=

FORMS +=

TRANSLATIONS = lang/Translation_en.ts \
    lang/Translation_de.ts


CONFIG(release, debug|release) {
    DESTDIR = release
} else {
    DESTDIR = debug
}

win32 {
    copyfiles.commands += @call xcopy /Y /I ..\\GIF-LZW\\lang\\*.* $${DESTDIR}\\lang\\ && @call xcopy /Y /I ..\\GIF-LZW\\rsc\\*.* $${DESTDIR}\\rsc\\
}
unix{
    copyfiles.commands += cp -rf ../GIF-LZW/lang $${DESTDIR} $$escape_expand(\n\t) cp -rf ../GIF-LZW/rsc $${DESTDIR}
}

QMAKE_EXTRA_TARGETS += copyfiles
POST_TARGETDEPS += copyfiles
