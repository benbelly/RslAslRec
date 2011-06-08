
#ifndef ASLALG_H__
#define ASLALG_H__


#include "cvsl/cvsl.h"
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

    void AddTruthTestC( int fnum, int h1NumPts, Pointer h1Pts,
                                  int h2NumPts, Pointer h2Pts );

    /*
     * Find the keyframes using threshold T1
     * report back on the differences between frames
     * to allow determination of best threshold
     */
    void findKeyframesC( int T1, Pointer differences );

    /*
     * Get the number of keyFrames - call AFTER findKeyframesC
     */
    int numKeyframesC();

    /*
     * Get the array of keyframe ids (must be pre-allocated)
     * Call AFTER findKeyframesC
     */
    void getKeyframeIdsC( Pointer ids );

    /*
     * Run the initialSDsC command to execute the first 
     * function in the difference image generation
     */
    void initialSDsC( Pointer ids, Pointer grays, int grayCount,
                      Pointer keyIds, Pointer keys, int keyCount,
                      int width, int height, int type );

    /*
     * Mask a difference image with the skin mask
     */
    void skinmaskSDC( int id, Pointer sd, Pointer skin,
                      int width, int height, int type );

    /*
     * Edgedetect a difference image and remove those pixels
     */
    void edgeAndMaskSDC( int id, Pointer sd, int w, int h, int t );

    /*
     * RemoveSmallComponents from a difference image
     */
    void removeSmallComponentsC( int id, Pointer sd, int w, int h, int t );

    /*
     * Extract the boundary image for a difference image
     */
    void extractBoundaryC( int id, Pointer sd, int w, int h, int t );

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
    void addHandsToSeqC( Pointer seqPtr,
                         int width, int height,
                         Pointer faceCorners,
                         int h1NumPts, Pointer h1Pts,
                         int h2NumPts, Pointer h2Pts );

    /*
     * Return the number of hand candidates detected in test frame
     */
    int numHandsC( int width, int height,
                   int type, Pointer pts );
    /*
     * Return the Mahalanobis distance of each hand candidate to the
     * ground truth hand (provided)
     */
    void distancesC( Pointer mDistances,
                     int width, int height,
                     int trainNum, Pointer trainPts,
                     int type, Pointer testImg );
    /*
     * Return the x,y coordinate of each hand candidate's center
     */
    void centersC( double *xs, double *ys,
                   int w, int h,
                   int trainNum, int *trainPts,
                   int type, char *img );

}

#endif
