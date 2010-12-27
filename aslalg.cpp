
#include "FrameDB.h"
#include "TrainDB.h"
#include "aslalg.h"

#include <string>
#include <string.h>
#include <iostream>
#include <map>

using std::string;
using std::cerr;
using std::endl;

// Special database of training images
typedef std::map<int, cv::Mat> TrainingImageMap;
static TrainingImageMap trainingImages;

/*
 * Initialize the database of frames using the specified video file
 * This is not used since the data is already broken into frames
 */
void InitAslAlgC( char *cfile, int filenameLen ) {
    FrameSet vidFrames = loadFromVideo( std::string( cfile, filenameLen ) );
    new FrameDB( vidFrames );
    new TrainDB();
}

/*
 * Run the findhands algorithm on the FrameDB already initialized
 * InitAslAlg() must be called before this
 */
void findHandsC() {
    FDB->findHands();
    FrameSet sds = FDB->sds();
}

/*
 * Get the number of frames of a particular type
 * InitAslAlg must be called first
 */
int numFramesC( int type ) {
    switch( type ) {
        case frame_types::special:
            return trainingImages.size();
        default:
            return FDB->size();
    }
}

// std::map doesn't have a 'keys' method so I use std::transform
// and this helper
int toKey( TrainingImageMap::value_type p ) { return p.first; }
/*
 * Get the ids associated with frames of a particular type
 * InitAslAlg must be called first
 */
void getFrameIdsC( int type, Pointer ids ) {
    switch( type ) {
        case frame_types::special:
        {
            std::vector<int> dbIds; dbIds.reserve( trainingImages.size() );
            std::transform( trainingImages.begin(), trainingImages.end(),
                            std::back_inserter( dbIds ), std::ptr_fun( toKey ) );
            std::copy( dbIds.begin(), dbIds.end(), (int*)ids );
        }
        default:
        {
            std::vector<int> dbIds( FDB->ids() );
            std::copy( dbIds.begin(), dbIds.end(), (int*)ids );
        }
    }
}

/*
 * Return the size (in bytes) of a frame
 */
int getFrameSize( const Frame &f ) {
    return f.size().height * f.size().width * f.mat.elemSize();
}

/*
 * Return the width, height, cv image type, and size of a type of images
 */
void getFrameInfoC( int type, Pointer width, Pointer height,
                    Pointer dtype, Pointer size ) {
    Frame toCheck;
    switch( type ) {
        case frame_types::original:
            toCheck = FDB->original( 0 );
            break;
        case frame_types::histogram:
            toCheck = FDB->histogram( 0 );
            break;
        case frame_types::special: // Meaningless
            toCheck = Frame( 0, trainingImages[0] );
            break;
        default:
            toCheck = FDB->gray( 0 );
            break;
    }
    *((int*)width) = toCheck.size().width;
    *((int*)height) = toCheck.size().height;
    *((int*)dtype) = toCheck.type();
    *((int*)size) = getFrameSize( toCheck );
}

bool isColor( int type ) {
    switch( type ) {
        case frame_types::original:
            return FDB->original( 0 ).channels() >= 3;
            break;
        default:
            return false;
            break;
    }
}

/*
 * Helper function for getFrameC that accesses/creates the requested frame
 */
Frame getFrame( int id, int type ) {
    switch( type ) {
        case frame_types::original:
            return FDB->original( id );
        case frame_types::gray:
            return FDB->gray( id );
        case frame_types::skin:
            return FDB->skin( id );
        case frame_types::sd:
            return FDB->sd( id );
        case frame_types::boundary:
            return FDB->boundary( id );
        case frame_types::histogram:
            return FDB->histogram( id );
        case frame_types::special:
            return Frame( id, trainingImages[id] );
    }
    return Frame();
}

/*
 * Return the image with id and type
 * InitAslAlg() must be called first
 */
void getFrameC( int id, int type, Pointer img ) {
    Frame get = getFrame( id, type );
    if( get.mat.isContinuous() )
        memcpy( img, get.mat.datastart, get.mat.dataend - get.mat.datastart );
    else
        cerr << "HELP! - Non-continuous image requested" << endl;
}

/*
 * Helper function for 
 * The input data skips scan lines and does weird things
 * This tries to correct for _most_ of it.
 */
void makeImageFromData( cv::Mat &img, int numPts, int *pts ) {
    int i = 0, x = pts[i++], y = pts[i++];
    // Since y jumps around, track the jump and increment actualY
    // Sometimes Y changes (incorrectly) in the middle of scanlines,
    // so track X position as well, and only increment y when both change
    int actualY = y, lastX, lastY = y;
    while( i < numPts ) {
        img.at<unsigned char>( actualY, x ) = 255;
        lastX = x;
        x = pts[i++]; y = pts[i++];
        if( y != lastY &&
            x != (lastX + 1) ) {
            lastY = y; ++actualY;
        }
    }
}

/*
 * Get a pointer to the next sequence for this gloss
 * InitAslAlg() must be called first
 */
Pointer seqForGlossC( int glossLen, Pointer glossPtr ) {
    std::string gloss( (char *)glossPtr, glossLen );
    return (Pointer) TDB->NextSequenceForGloss( gloss );
}

int addHandsToSeqC( Pointer seqPtr,
                    int width, int height,
		    int h1NumPts, Pointer h1Pts,
		    int h2NumPts, Pointer h2Pts ) {
    cv::Mat hand = cv::Mat::zeros( height, width, image_types::gray ),
            weak = cv::Mat::zeros( height, width, image_types::gray );
    makeImageFromData( hand, h1NumPts, (int *)h1Pts );
    if( h2NumPts ) makeImageFromData( weak, h2NumPts, (int *)h2Pts );
    SignSeq *seq = (SignSeq *)seqPtr;
    seq->AddHands( hand, weak );
    return 0;
}

/*
 *int addHandImageC( int glossLen, Pointer glossPtr,
 *                   int width, int height,
 *                   int h1NumPts, Pointer h1Pts,
 *                   int h2NumPts, Pointer h2Pts ) {
 *    std::string gloss( glossPtr, glossLen );
 *    cv::Mat hand = cv::Mat::zeros( height, width, image_types::gray );
 *    makeImageFromData( hand, h1NumPts, (int *)h1Pts );
 *    if( h2NumPts ) makeImageFromData( hand, h2NumPts, (int *)h2Pts );
 *    static int lastIndex = 0;
 *    trainingImages[lastIndex] = hand;
 *    return lastIndex++;
 *}
 */

