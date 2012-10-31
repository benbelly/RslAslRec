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
