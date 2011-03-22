
#include "Databases.h"
#include "aslalg.h"
#include "differenceImageFunctions.h"
#include "cvsl/cvhelpers.h"

#include <string>
#include <string.h>
#include <iostream>
#include <map>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

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
 * Find the keyframes using threshold T1
 * report back on the differences between frames
 * to allow determination of best threshold
 */
void findKeyframesC( int t1, Pointer differences ) {
    std::vector<double> diffs = FDB->findKeyframes( t1 );
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
 * Run the initialSDsC command to execute the first 
 * function in the difference image generation
 */
void initialSDsC( Pointer ids, Pointer grays, int grayCount,
                  Pointer keyIds, Pointer keys, int keyCount,
                  int width, int height, int type ) {
    FrameSet grayFrames = loadFromByteArray( (int*)ids, (char*)grays,
                                             grayCount,
                                             width, height, type );
    FrameSet keyFrames = loadFromByteArray( (int*)keyIds, (char*)keys,
                                            keyCount,
                                            width, height, type );
    FrameSet sds = generateInitialSDs( grayFrames, keyFrames );
    FDB->setAllSDs( sds );
}

/*
 * Mask a difference image with the skin mask
 */
void skinmaskSDC( int id, Pointer sd, Pointer skin,
                  int width, int height, int type ) {
    FrameSet sds = loadFromByteArray( &id, (char *)sd, 1,
                                      width, height, type );
    FrameSet skins = loadFromByteArray( &id, (char *)skin, 1,
                                        width, height, type );
    Frame sdFrame = sds[0], skinFrame = skins[0];
    cv::Mat masked;
    sdFrame.mat.copyTo( masked, skinFrame.mat );
    FDB->setSingleSD( Frame( id, masked ) );
}

/*
 * Edgedetect a difference image and remove those pixels
 */
void edgeAndMaskSDC( int id, Pointer sd, int w, int h, int t ) {
    FrameSet sds = loadFromByteArray( &id, (char*)sd, 1, w, h, t );
    Frame sdFrame = sds[0];
    Frame edge = getEdge( sdFrame, true );
    Frame negated = negateAndMaskFrame( sdFrame, edge );
    FDB->setSingleSD( negated );
}

/*
 * RemoveSmallComponents from a difference image
 */
void removeSmallComponentsC( int id, Pointer sd, int w, int h, int t ) {
    FrameSet sds = loadFromByteArray( &id, (char*)sd, 1, w, h, t );
    Frame sdFrame = sds[0];
    Frame removed = removeComponentsFromFrame( sdFrame );
    FDB->setSingleSD( removed );
}

/*
 * Extract the boundary image for a difference image
 */
void extractBoundaryC( int id, Pointer sd, int w, int h, int t ) {
    FrameSet sds = loadFromByteArray( &id, (char*)sd, 1, w, h, t );
    Frame sdFrame = sds[0];
    std::pair<Frame, ContourSet> boundary = getBoundaryImage( sdFrame );
    FDB->setBoundary( boundary.first );
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
 * Helper function for creating images from the training data
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

/* Create a hand from the list of points */
cv::Mat makeHandFromPointList( int width, int height,
                               int numPts, Pointer pts ) {
    cv::Mat hand = cv::Mat::zeros( height, width, image_types::gray );
    makeImageFromData( hand, numPts, (int *)pts );
    return hand;
}
                            
/*
 * Add hands and face to the provided sign sequence
 * InitAslAlg() must be called first, and seqPtr should
 * be provided by seqForGlossC()
 */
void addHandsToSeqC( Pointer seqPtr,
                     int width, int height,
                     Pointer faceCorners,
                     int h1NumPts, Pointer h1Pts,
                     int h2NumPts, Pointer h2Pts ) {
    cv::Mat hand = cv::Mat::zeros( height, width, image_types::gray );
    boost::shared_ptr<cv::Mat> weak( (cv::Mat *)0 );
    makeImageFromData( hand, h1NumPts, (int *)h1Pts );
    if( h2NumPts ) {
        weak.reset( new cv::Mat( cv::Mat::zeros( height, width, image_types::gray ) ) );
        makeImageFromData( *(weak.get()), h2NumPts, (int *)h2Pts );
    }
    int *facePts = (int *)faceCorners;
    cv::Point topLeft( facePts[0], facePts[1] ), bottomRight( facePts[2], facePts[3] );
    SignSeq *seq = (SignSeq *)seqPtr;
    seq->AddHands( topLeft, bottomRight, hand, weak );
}

/*
 * Return the number of hand candidates detected in test frame
 */
int numHandsC( int w, int h, int type, Pointer imgArr ) {
    cv::Mat img = makeMat( imgArr, w, h, type );
    return getHands( img ).size();
}

/*
 * Return the Mahalanobis distance of each hand candidate to the
 * ground truth hand (provided)
 */
void distancesC( Pointer mDistances,
                 int width, int height,
                 int trainNum, Pointer trainPts,
                 int type, Pointer testImg ) {
    cv::PCA pca = TDB->Pca();
    cv::Mat train = makeHandFromPointList( width, height, trainNum, trainPts );
    Histogram trainHist = generateHandHistogram( train.size(), getBoundary( train ) );
    cv::Mat trainProj = pca.project( trainHist );

    cv::Mat icovar = TDB->Covariance();

    cv::Mat testSrc = makeMat( testImg, width, height, type );
    ContourSet testHands = getHands( testSrc );
    HistogramSet hs = generateHandHistograms( testSrc.size(), testHands );
    ProjectionSet ps; ps.reserve( testHands.size() );
    std::transform( hs.begin(), hs.end(), std::back_inserter( ps ),
                    boost::bind( &cv::PCA::project, pca, _1 ) );
    std::transform( ps.begin(), ps.end(), (double *)mDistances,
                    boost::bind( cv::Mahalanobis, _1, trainProj, icovar ) );
}

/*
 * Return the x,y coordinate of each hand candidate's center
 */
float getX( CenterPoint &p ) { return p.x; }
float getY( CenterPoint &p ) { return p.y; }
void centersC( Pointer xs, Pointer ys,
               int w, int h, int type,
               Pointer imgArr ) {
    cv::Mat img = makeMat( imgArr, w, h, type );
    ContourSet contours = getHands( img );
    CenterSet cs = centers( contours );
    std::transform( cs.begin(), cs.end(), (float *)xs,
                    boost::bind( getX, _1 ) );
    std::transform( cs.begin(), cs.end(), (float *)ys,
                    boost::bind( getY, _1 ) );
}
