
#include <algorithm>
#include <iterator>

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv/cvaux.h"

#include "logging.h"
#include "skinmask.h"

SkinMask generateSkinMask( Frame frame ) {
    cv::Mat maskMat( frame.size(), CV_8UC1 );
    IplImage srcImg = (IplImage)frame.mat,
             mask = (IplImage)maskMat;
    CvAdaptiveSkinDetector filter( 1, CvAdaptiveSkinDetector::MORPHING_METHOD_ERODE_DILATE );
    filter.process( &srcImg, &mask );
    return SkinMask( frame, maskMat );
}

SkinMaskSet generateSkinMasks( FrameSet &frames ) {
    SkinMaskSet masks; masks.reserve( frames.size() );
    std::transform( frames.begin(), frames.end(), std::back_inserter( masks ),
                    std::ptr_fun( generateSkinMask ) );
    return masks;
}

Frame maskFrame( Frame &f, SkinMask &sk ) {
    Frame masked( f.id, f.size(), f.type() );
    f.mat.copyTo( masked.mat, sk.mask );
    return masked;
}
