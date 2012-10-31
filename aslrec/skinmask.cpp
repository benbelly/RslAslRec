/**
    Copyright 2011 Ben Holm

    This file is part of RslAslRec.

    RslAslRec is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RslAslRec is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RslAslRec.  If not, see <http://www.gnu.org/licenses/>.
**/


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
