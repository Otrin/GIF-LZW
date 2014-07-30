INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    src/entropy/huffmannode.h \
    src/entropy/huffman.h \
    src/entropy/runlengthencoding.h \
    src/entropy/node.h \
    src/entropy/huffman2.h

SOURCES += \
    src/entropy/huffmannode.cpp \
    src/entropy/huffman.cpp \
    src/entropy/runlengthencoding.cpp \
    src/entropy/node.cpp \
    src/entropy/huffman2.cpp
