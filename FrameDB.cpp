
#include <numeric>

#include "logging.h"

#include "keyframeselect.h"
#include "keyframedist.h"
#include "skinmask.h"
#include "edgedetection.h"
#include "FrameDB.h"
#include "utility.h"

#include "FrameDBHelpers.h"

FrameDB *FDB;

extern "C" {
    cv::Mat original( int );
    cv::Mat gray( int );
}

FrameDB::FrameDB( string vFile ) : videoFile( vFile ) {
    FDB = this;
}

void FrameDB::findHands() {
    loadVideo();
    findKeyframes();
    makeSDs();
}

FrameSet FrameDB::getItem( FrameDB::Accessor f ) const {
    FrameSet os; os.reserve( db.size() );
    std::transform( db.begin(), db.end(), std::back_inserter( os ),
                    std::ptr_fun( f ) );
    return os;
}

void FrameDB::setItem( FrameDB::Setter s, FrameSet vals ) {
    TwoFor( db.begin(), db.end(), vals.begin(), vals.end(),
            std::ptr_fun( s ) );
}

FrameSet FrameDB::originals() const {
    return getItem( getOriginal );
}

FrameSet FrameDB::grays() const {
    return getItem( getGray );
}

FrameSet FrameDB::skins() const {
    return getItem( getSkin );
}

FrameSet FrameDB::sds() const {
    return getItem( getSD );
}

FrameDB::FrameData::FrameData( int i, const cv::Mat &img ) :
    id( i ),
    original( i ),
    gray( i, img.size(), image_types::gray )
{
    img.copyTo( original.mat );
    skinMask = generateSkinMask( original );
    cv::cvtColor( img, gray.mat, CV_BGR2GRAY );
}

void FrameDB::loadVideo() {
    cv::VideoCapture cap( videoFile );

    int i = 0;
    cv::Mat img;
    while( cap.grab() ) {
        cap.retrieve( img );
        db[i] = FrameData( i, img );
        ++i;
    }
}

void FrameDB::findKeyframes() {
    FrameSet graySet = grays();
    Frame firstKeyframe = graySet[0];
    keyframes.push_back( firstKeyframe );
    std::accumulate( graySet.begin() + 1, graySet.end(),
                     firstKeyframe,
                     AccumKeyframes( keyframes ) );
    std::cout << "Keyframe count = " << keyframes.size() << " of " << db.size() << " frames"
              << std::endl;
}

void FrameDB::makeSDs() {
    FrameSet init = generateInitialSDs();
    FrameSet SDs = maskedSDs( init );
    FrameSet edges = getDilatedEdges( SDs );
    FrameSet negated = negateAndMask( SDs, edges );
    FrameSet cleaned = removeSmallConnectedComponents( negated );
    setItem( setSD, cleaned );
}

FrameSet FrameDB::generateInitialSDs() {
    FrameSet SDs; SDs.reserve( db.size() );
    FrameSet frames = grays();
    FrameSet bigKeys = gray8bitTogray16bit( keyframes );
    std::transform( frames.begin(), frames.end(), std::back_inserter( SDs ),
                    std::bind1st( std::ptr_fun( avgDist2 ), keyframes ) );
    return SDs;
}
  
FrameSet FrameDB::maskedSDs( FrameSet SDs ) {
    FrameSet skinMasked; skinMasked.reserve( SDs.size() );
    FrameSet skinMasks = skins();
    Zip( SDs.begin(), SDs.end(), skinMasks.begin(), skinMasks.end(),
         std::back_inserter( skinMasked ), maskFrame );
    return skinMasked;
}
