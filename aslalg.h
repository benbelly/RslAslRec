
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
        training = 6
    };
}

// API for sml
extern "C" {

    /*
     * Initialize the database of frames using the specified video file
     * This is not used since the data is already broken into frames
     */
    void InitAslAlgC( char **files, int *filelens, int *frameIds, int numFiles );
    /*
     * Run the findhands algorithm on the FrameDB already initialized
     * InitAslAlg() must be called before this
     */
    void findHandsC();

    /*
     * Get a pointer to the next sequence for this gloss
     * InitAslAlg() must be called first
     */
    Pointer seqForGlossC( int glossLen, Pointer glossPtr );

    /*
     * Add hands and face to the provided sign sequence
     * InitAslAlg() must be called first, and seqPtr should
     * be provided by seqForGlossC()
     */
    int addHandsToSeqC( Pointer seqPtr,
                        int width, int height,
                        Pointer faceCorners,
                        int h1NumPts, Pointer h1Pts,
                        int h2NumPts, Pointer h2Pts );

}

#endif
