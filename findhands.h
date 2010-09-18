
#ifndef FINDHANDS_H__
#define FINDHANDS_H__

// ML Stuff
#include "export.h"
#include "ml-types.h"

// API for sml
extern "C" {
    int numFrames();
    void getFrameIds( Pointer ids );

    void getFrameInfoC( int type, Pointer width, Pointer height,
                                 Pointer dtype, Pointer size );
    void getVideoInfoC( int type, Pointer color, Pointer fourcc, Pointer fps );
    void getFrame( int id, int type, Pointer img );
}

namespace frame_types {
    enum {
        original = 0,
        gray = 1,
        skin = 2,
        sd = 3,
        boundary = 4
    };
}

extern "C" {
    void InitSarkur( char *cfile, int filenameLen );
    void findHands();
}

#endif
