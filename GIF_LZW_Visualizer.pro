#-------------------------------------------------
#
# Project created by QtCreator 2014-05-28T09:01:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GIF_LZW_Visualizer
TEMPLATE = app


include(src/gui/gui.pri)
include(src/lzw/lzw.pri)
include(src/globaltablecomparison/globaltablecomparison.pri)

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
    copyfiles.commands += @call xcopy /Y ..\\GIF-LZW\\lang\\*.* $${DESTDIR}\\lang\\ $$escape_expand(\n\t) @call xcopy /Y ..\\GIF-LZW\\rsc\\*.* $${DESTDIR}\\rsc\\
}
unix{
    copyfiles.commands += cp -rf ../GIF-LZW/lang $${DESTDIR} $$escape_expand(\n\t) cp -rf ../GIF-LZW/rsc $${DESTDIR}
}

QMAKE_EXTRA_TARGETS += copyfiles
POST_TARGETDEPS += copyfiles
