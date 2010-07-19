
#ifndef SKINMASK_H__
#define SKINMASK_H__

#include <vector>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "frame.h"

struct SkinMask {
    SkinMask( Frame &f, cv::Mat &m ) : frame( f ), mask( m ) { }
    Frame frame; // frame being masked
    cv::Mat mask;
    void showNwait() { cv::imshow( "mask", mask ); cv::waitKey(); }
};
typedef std::vector<SkinMask> SkinMaskSet;

SkinMaskSet generateSkinMasks( FrameSet &frames );

Frame maskFrame( Frame &f, SkinMask &sk );
#endif
