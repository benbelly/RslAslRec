
#ifndef CVHELPERS_H__
#define CVHELPERS_H__

#include "export.h" // mlton types
#include "ml-types.h" // mlton types, for real
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <iostream>

using std::cerr;
using std::endl;

/*
 * Helper function to copy from an image point to a destination matrix.
 * Templated to allow different destination types
 */
template<typename MatType>
void CopyMat( Pointer img, MatType dst ) {
    if( dst.isContinuous() )
        memcpy( dst.datastart, img, dst.dataend - dst.datastart );
    else
        cerr << "Cannot copy image - destination is not continuous" << endl;
}

/*
 * Helper function to create a cv::Mat from the provided image data
 */
inline cv::Mat makeMat( Pointer img, int width, int height, int type ) {
    cv::Mat dst = cv::Mat::zeros( height, width, type );
    CopyMat( img, dst );
    return dst;
}

/*
 * Helper function to create a cv::Mat_<T> from the provided image data
 */
template<typename T>
cv::Mat_<T> makeMat( Pointer img, int width, int height ) {
    cv::Mat_<T> dst = cv::Mat_<T>::zeros( height, width );
    CopyMat( img, dst );
    return dst;
}


#endif
