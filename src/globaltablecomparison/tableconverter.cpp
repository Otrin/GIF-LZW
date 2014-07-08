#include "tableconverter.h"
#include "gif.h"

Gif TableConverter::globalToLocal(Gif p_gif){
    //TODO
    return Gif();
}

Gif TableConverter::localToGlobal(Gif p_gif){
    if(p_gif.getGctFlag())
        return p_gif;

    char* colorTable = new char[256];

    for (int i = 0; i < p_gif.getSizeOfFrames(); ++i) {
        p_gif.getFrame(i); //do stuff with it
    }

    return Gif();
}
