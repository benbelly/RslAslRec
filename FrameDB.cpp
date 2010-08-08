
#include <numeric>

#include "keyframeselect.h"
#include "FrameDB.h"

FrameDB::FrameDB( string vFile ) : videoFile( vFile ) {
    loadVideo();
    findKeyframes();
}

FrameSet FrameDB::getItem( FrameDB::Accessor f ) const {
    FrameSet os; os.reserve( db.size() );
    std::transform( db.begin(), db.end(), std::back_inserter( os ),
                    std::ptr_fun( f ) );
    return os;
}

Frame getOriginal( FrameDB::RowType p ) {
    return p.second.original;
}

FrameSet FrameDB::originals() const {
    return getItem( getOriginal );
}

Frame getGray( FrameDB::RowType p ) {
    return p.second.gray;
}

FrameSet FrameDB::grays() const {
    return getItem( getGray );
}

Frame getSkin( FrameDB::RowType p ) {
    return p.second.skinMask;
}

FrameSet FrameDB::skins() const {
    return getItem( getSkin );
}

Frame getSD( FrameDB::RowType p ) {
    return p.second.SD;
}

FrameSet FrameDB::sds() const {
    return getItem( getSD );
}

void FrameDB::loadVideo() {
    cv::VideoCapture cap( videoFile );

    int i = 0;
    cv::Mat img;
    while( cap.grab() ) {
        cap.retrieve( img );
        Frame color( i, img.size(), img.type() );
        img.copyTo( color.mat );
        Frame gray( i, img.size(), CV_8UC1 );
        cv::cvtColor( img, gray.mat, CV_RGB2GRAY );
        db[i] = FrameData( i++, color, gray );
    }
}

void FrameDB::findKeyframes() {
    keyframes.reserve( db.size() / 5 );

    keyframes.push_back( gray( 0 ) );
    FrameSet graySet = grays();
    std::accumulate( graySet.begin() + 1, graySet.end(),
                     keyframes[0],
                     AccumKeyframes( keyframes ) );
}
