
#include "Databases.h"
#include "aslalg.h"

#include <string>
#include <string.h>
#include <iostream>
#include <map>
#include <boost/bind.hpp>

using std::string;
using std::cerr;
using std::endl;

// Special database of training images
typedef std::map<int, cv::Mat> TrainingImageMap;
static TrainingImageMap trainingImages;

/*
 * Initialize the FrameDatabase and the TrainingDatabase. The FrameDatabase will need 
 * an array of filenames (frame images), an array of lengths, an array of frame ids
 * (usually the index) and a length that indicates the length of all arrays
 */
void InitAslAlgC( char **files, int *filelens, int *frameIds, int numFiles ) {
    std::vector<std::pair<std::string, int> > frameFiles; frameFiles.reserve( numFiles );
    for( int i = 0; i < numFiles; ++i )
        frameFiles.push_back(
                std::make_pair( std::string( files[i], filelens[i] ), frameIds[i] ) );
    FrameSet vidFrames = loadFromFiles( frameFiles );
    new FrameDB( vidFrames );
    new TrainDB();
}

/*
 * Find the keyframes - report back on the differences between frames
 * to allow determination of best threshold
 */
void findKeyframesC( Pointer differences ) {
    std::vector<double> diffs = FDB->findKeyframes();
    memcpy( differences, &diffs[0], diffs.size() * sizeof( double ) );
}

/*
 * Get the number of keyFrames
 */
int numKeyframesC() {
    //cout << "number of keyframes: " << FDB->keys().size() << endl;
    return FDB->keys().size();
}

/*
 * Get the array of keyframe ids (must be pre-allocated)
 */
void getKeyframeIdsC( Pointer ids ) {
    FrameSet keys = FDB->keys();
    std::vector<int> idVec; idVec.reserve( keys.size() );
    std::transform( keys.begin(), keys.end(), std::back_inserter( idVec ),
                    boost::bind( &Frame::Id, _1 ) );
    memcpy( ids, &idVec[0], idVec.size() * sizeof( int ) );
}

/*
 * Run the findhands algorithm on the FrameDB already initialized
 * InitAslAlg() must be called before this
 */
void findHandsC() {
    FDB->findHands();
}

/*
 * Get the number of frames of a particular type
 * InitAslAlg must be called first
 */
int numFramesC( int type ) {
    switch( type ) {
        case frame_types::training:
            return TDB->ids().size();
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
        case frame_types::training:
        {
            std::vector<int> dbIds( TDB->ids() );
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
int getFrameSize( const cv::Mat &f ) {
    return f.size().height * f.size().width * f.elemSize();
}

/*
 * Return the width, height, cv image type, and size of a type of images
 */
void getFrameInfoC( int type, Pointer width, Pointer height,
                    Pointer dtype, Pointer size ) {
    cv::Mat toCheck;
    int id = FDB->ids()[0];
    switch( type ) {
        case frame_types::original:
            toCheck = FDB->original( id ).mat;
            break;
        case frame_types::histogram:
            toCheck = FDB->histogramImg( id ).mat;
            break;
        case frame_types::training:
            {
                id = TDB->ids()[0];
                toCheck = TDB->GetFeatureFrame( id )->dom;
                break;
            }
        default:
            toCheck = FDB->gray( id ).mat;
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
            return FDB->histogramImg( id );
        case frame_types::training:
            return Frame( id, TDB->GetFeatureFrame( id )->dom );
    }
    return Frame();
}

/*
 * Return the image with id and type
 * InitAslAlg() must be called first
 */
void getFrameC( int id, int type, Pointer img ) {
    cv::Mat get = getFrame( id, type ).mat;
    if( get.isContinuous() )
        memcpy( img, get.datastart, get.dataend - get.datastart );
    else {
        //cv::imwrite("cvsl_out/foo.png", get, std::vector<int>());
        // Can't copy the entire image, copy row-by-row
        int step = get.elemSize() * get.cols,
            last = 0;
        for( int y = 0; y < get.rows; ++y ) {
            char *rowPtr = get.ptr<char>( y );
            memcpy( img + last, rowPtr, step );
        }
    }
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

/*
 * Add hands and face to the provided sign sequence
 * InitAslAlg() must be called first, and seqPtr should
 * be provided by seqForGlossC()
 */
int addHandsToSeqC( Pointer seqPtr,
                    int width, int height,
                    Pointer faceCorners,
                    int h1NumPts, Pointer h1Pts,
                    int h2NumPts, Pointer h2Pts ) {
    cv::Mat hand = cv::Mat::zeros( height, width, image_types::gray ),
            weak = cv::Mat::zeros( height, width, image_types::gray );
    makeImageFromData( hand, h1NumPts, (int *)h1Pts );
    if( h2NumPts ) makeImageFromData( weak, h2NumPts, (int *)h2Pts );
    int *facePts = (int *)faceCorners;
    cv::Point topLeft( facePts[0], facePts[1] ), bottomRight( facePts[2], facePts[3] );
    SignSeq *seq = (SignSeq *)seqPtr;
    seq->AddHands( topLeft, bottomRight, hand, weak );
    return 0;
}

