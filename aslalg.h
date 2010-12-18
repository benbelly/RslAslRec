
#ifndef FINDHANDS_H__
#define FINDHANDS_H__

// ML Stuff
#include "export.h"
#include "ml-types.h"

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
     * Get the number of frames of a particular type
     * InitAslAlg() must be called first
     */
    int numFramesC( int type );
    /*
     * Get the ids associated with frames of a particular type
     * InitAslAlg() must be called first
     */
    void getFrameIdsC( int type, Pointer ids );

    /*
     * Return the width, height, cv image type, and size of a type of images
     * InitAslAlg() must be called first
     */
    void getFrameInfoC( int type, Pointer width, Pointer height,
                                 Pointer dtype, Pointer size );
    /*
     * Get information about the input video
     * InitAslAlg() must be called first
     */
    void getVideoInfoC( int type, Pointer color, Pointer fourcc, Pointer fps );
    /*
     * Return the image with id and type
     * InitAslAlg() must be called first
     */
    void getFrameC( int id, int type, Pointer img );

    /*
     * Add a training image - assumes a data format
     * InitAslAlg() must be called first
     */
    int addHandImageC( int width, int height,
                      int h1NumPts, Pointer h1Pts,
                      int h2NumPts, Pointer h2Pts );

}

#endif
