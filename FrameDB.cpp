
#include<iostream>
#include<numeric>

//#include "logging.h"

#include "keyframeselect.h"
#include "keyframedist.h"
#include "skinmask.h"
#include "edgedetection.h"
#include "FrameDB.h"
#include "utility.h"

#include "FrameDBHelpers.h"

using std::cout;
using std::endl;

FrameDB *FDB;

FrameDB::FrameDB( FrameSet &os ) {
    for( unsigned int i = 0; i < os.size(); ++i )
        db[i] = FrameData( i, os[i].mat );
    FDB = this;
}

void FrameDB::findHands() {
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

std::vector<int> FrameDB::ids() const {
    std::vector<int> idvec;
    std::transform( db.begin(), db.end(), std::back_inserter( idvec ),
                    std::ptr_fun( getId ) );
    return idvec;
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

FrameSet FrameDB::boundaries() const {
    return getItem( getBoundary );
}

FrameSet FrameDB::keys() const {
     return keyframes;
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
    FrameHandSet boundaries = getBoundaryImages( cleaned );
    for( FrameHandSet::iterator i = boundaries.begin(); i != boundaries.end(); ++i ) {
        db[i->first.id].boundary = i->first;
        db[i->first.id].hands = i->second;
        db[i->first.id].histograms = generateHandHistograms( i->first, i->second );
    }
}

Frame FrameDB::histogram( int i ) {
    cv::Mat img = h2i( db[i].gray, db[i].histograms );
    return Frame( i, img );
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
