
#include "frame.h"
#include <map>
#include <algorithm>

FrameSet loadFromVideo( std::string videoFile ) {
    cv::VideoCapture cap( videoFile );
    /*
     *fourcc = cap.get( CV_CAP_PROP_FOURCC );
     *fps = cap.get( CV_CAP_PROP_FPS );
     */
    int numFrames = cap.get( CV_CAP_PROP_FRAME_COUNT );
    FrameSet frames; frames.reserve( numFrames );

    int i = 0;
    cv::Mat img;
    while( cap.grab() ) {
        cap.retrieve( img );
        cv::Mat copy( img.size(), img.type() );
        img.copyTo( copy );
        frames.push_back( Frame( i++, copy ) );
    }
    return frames;
}

Frame convertTo16bit( const Frame src ) {
    Frame dst( src.id, src.size(), image_types::big_gray );
    src.mat.convertTo( dst.mat, image_types::big_gray );
    return dst;
}

FrameSet gray8bitTogray16bit( const FrameSet &src ) {
    FrameSet bigs; bigs.reserve( src.size() );
    std::transform( src.begin(), src.end(), std::back_inserter( bigs ),
                    std::ptr_fun( convertTo16bit ) );
    return bigs;
}

int Frame::Id() const { return id; }
cv::Size Frame::size() const { return mat.size(); }
int Frame::type() const { return mat.type(); }
int Frame::depth() const { return mat.depth(); }
int Frame::channels() const { return mat.channels(); }
