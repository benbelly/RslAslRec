
#include <algorithm>
#include <iterator>

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv/cvaux.h"

#include "skinmask.h"
#include "SkinModel.h"

Frame generateSkinMask( Frame frame ) {
    SkinModel sm;
    return sm.GetSkinMask( frame );
    /*
     *cv::Mat maskMat = cv::Mat::zeros( frame.size(), image_types::gray );
     *IplImage srcImg = (IplImage)frame.mat,
     *         mask = (IplImage)maskMat;
     *CvAdaptiveSkinDetector filter( 1, CvAdaptiveSkinDetector::MORPHING_METHOD_ERODE_DILATE );
     *filter.process( &srcImg, &mask );
     *return Frame( frame.id, maskMat );
     */
}

FrameSet generateSkinMasks( FrameSet &frames ) {
    FrameSet masks; masks.reserve( frames.size() );
    std::transform( frames.begin(), frames.end(), std::back_inserter( masks ),
                    std::ptr_fun( generateSkinMask ) );
    return masks;
}

Frame maskFrame( Frame &f, Frame &sk ) {
    cv::Mat masked = cv::Mat::zeros( f.size(), f.type() );
    f.mat.copyTo( masked, sk.mat );
    /*
     *cv::imshow( "gray", f.mat );
     *cv::imshow( "skin", sk.mat );
     *cv::imshow( "masked", masked );
     *cv::waitKey();
     */
    return Frame ( f.id, masked );
}
