
#include "FrameDB.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "cvsl.h"

void showImage( Pointer img, int width, int height, int type, Bool wait ) {
    cv::Mat dst = cv::Mat::zeros( width, height, type );
    //std::copy( img, img + imgSize, dst.datastart );
    for( Pointer i = img, d = dst.datastart; d != dst.dataend; ++i, ++d )
        *d = *i;

    cv::imshow( "Image", dst );
    if( wait ) cv::waitKey();
}

