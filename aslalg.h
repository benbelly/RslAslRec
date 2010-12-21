
#ifndef FINDHANDS_H__
#define FINDHANDS_H__

// ML Stuff
#include "export.h"
#include "ml-types.h"

#include "cvsl.h"

namespace frame_types {
    enum {
        original = 0,
        gray = 1,
        skin = 2,
        sd = 3,
        boundary = 4,
        histogram = 5,
        special = 6
    };
}

// API for sml
extern "C" {

    /*
     * Initialize the database of frames using the specified video file
     * This is not used since the data is already broken into frames
     */
    void InitAslAlgC( char *cfile, int filenameLen );
    /*
     * Run the findhands algorithm on the FrameDB already initialized
     * InitAslAlg() must be called before this
     */
    void findHandsC();

    /*
     * Add a training image - assumes a data format
     * InitAslAlg() must be called first
     */
    int addHandImageC( int width, int height,
                       int h1NumPts, Pointer h1Pts,
                       int h2NumPts, Pointer h2Pts );
}

#endif
