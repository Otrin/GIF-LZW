INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    src/entropy/runlengthencoding.h \
    src/entropy/node.h \
    src/entropy/compressorworker.h \
    src/entropy/huffman.h

SOURCES += \
    src/entropy/runlengthencoding.cpp \
    src/entropy/node.cpp \
    src/entropy/compressorworker.cpp \
    src/entropy/huffman.cpp
