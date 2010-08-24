
#include <sstream>
#include <vector>

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv/cvaux.h"

using std::vector;

int main( int, char **argv ) {
    cv::VideoCapture cap( argv[1] );

    cv::Mat img;
    vector<cv::Mat> mats, grays;

    while( cap.grab() ) {
        cap.retrieve( img );
        cv::Mat grab = cv::Mat::zeros( img.size(), img.type() );
        img.copyTo( grab );
        mats.push_back( grab );
        cv::Mat gray = cv::Mat::zeros( img.size(), img.type() );
        cv::cvtColor( img, gray, CV_BGR2GRAY );
        grays.push_back( gray );
    }

    for( unsigned int i = 0; i < grays.size(); ++i ) {
        cv::Mat b = cv::Mat::zeros( grays[i].size(), grays[i].type() );
        grays[i].convertTo( b, CV_16UC1, 255 );
        cv::imshow( "mask", b );
        cv::waitKey();
    }
}
