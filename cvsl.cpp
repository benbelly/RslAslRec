
#include "FrameDB.h"


#include "opencv/cv.h"
#include "opencv/highgui.h"

extern "C" {
    void showImage( int i );
}

void showImage( int i ) {
    cv::imshow( "cvsl", FDB->original( i ).mat );
    cv::waitKey();
}
