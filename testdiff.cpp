
#include <sstream>
#include <vector>

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv/cvaux.h"

using std::vector;

vector<cv::Mat> getSkins( vector<cv::Mat> &mats ) {
    vector<cv::Mat> masks;

    CvAdaptiveSkinDetector filter;

    for( int i = 0; i < mats.size(); ++i ) {
        cv::Mat maskMat( mats[i].size(), CV_8UC1 );
        IplImage src = (IplImage)mats[i],
                 msk = (IplImage)maskMat;
        filter.process( &src, &msk );
        masks.push_back( maskMat );
    }
    return masks;
}

int main( int, char **argv ) {
    cv::VideoCapture cap( argv[1] );

    cv::Mat img;
    vector<cv::Mat> mats;

    while( cap.grab() ) {
        cap.retrieve( img );
        cv::Mat grab( img.size(), img.type() );
        img.copyTo( grab );
        mats.push_back( grab );
    }

    vector<cv::Mat> masks = getSkins( mats );

    for( int i = 0; i < masks.size(); ++i ) {
        cv::imshow( "mask", masks[i] );
        cv::waitKey();
    }
}
