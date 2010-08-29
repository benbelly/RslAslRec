
#include "edgedetection.h"
#include "consts.h"
#include "FrameDB.h"
#include "utility.h"
#include "logging.h"

cv::Mat doDilate( cv::Mat mat ) {
    cv::Mat dilated = cv::Mat::zeros( mat.size(), mat.type() );
    cv::dilate( mat, dilated, cv::Mat( 3, 3, image_types::gray, cv::Scalar( 255 ) ) );
    return dilated;
}

Frame getEdge( Frame f, bool dilate ) {
    cv::Mat edges = cv::Mat::zeros( f.size(), f.type() );
    cv::Canny( f.mat, edges, 200, 255 );
    if( dilate )
        edges = doDilate( edges );
    return Frame( f.id, edges );
}

FrameSet getEdges( FrameSet &frames ) {
    FrameSet edged; edged.reserve( frames.size() );
    std::transform( frames.begin(), frames.end(),
                    std::back_inserter( edged ),
                    std::bind2nd( std::ptr_fun( getEdge ), false ) );
    return edged;
}

FrameSet getDilatedEdges( FrameSet &frames ) {
    FrameSet edged; edged.reserve( frames.size() );
    std::transform( frames.begin(), frames.end(),
                    std::back_inserter( edged ),
                    std::bind2nd( std::ptr_fun( getEdge ), true ) );
    return edged;
}

uchar maskPixel( uchar sd, uchar mask ) {
    return mask ? 0 : sd;
}

Frame negateAndMaskFrame( Frame SD, Frame mask ) {
    Frame result( SD.id, SD.size(), SD.type() );
    cv::Mat valid = cv::Mat::zeros( mask.mat.size(), mask.mat.type() );
    mask.mat.copyTo( valid, FDB->skin( SD.id ).mat );
    Zip( SD.mat.datastart, SD.mat.dataend, valid.datastart, valid.dataend,
         result.mat.datastart, maskPixel );
    //cv::imshow( "SD", SD.mat ); cv::imshow( "mask", valid ); cv::imshow("result", result.mat);
    //cv::waitKey();
    return result;
}

FrameSet negateAndMask( FrameSet &SDs, FrameSet &masks ) {
    FrameSet masked; masked.reserve( SDs.size() );
    Zip( SDs.begin(), SDs.end(), masks.begin(), masks.end(),
         std::back_inserter( masked ),
         negateAndMaskFrame );
    return masked;
}

bool IsSmallComponent( Contour m ) {
    //TRACEAREA( cv::contourArea( cv::Mat( m ) ) )
    return cv::contourArea( cv::Mat( m ) ) < SMALLCOMPONENT;
}

ContourSet getSmallContours( ContourSet &c ) {
    ContourSet small; small.reserve( c.size() );
    Copy_if( c.begin(), c.end(), std::back_inserter( small ),
             std::ptr_fun( IsSmallComponent ) );
    return small;
}

Frame removeComponentsFromFrame( Frame f ) {
    Frame clean( f.id, f.size(), f.type() );
    ContourSet contours;
    cv::Mat src = cv::Mat::zeros( f.size(), f.type() );
    f.mat.copyTo( src );
    cv::findContours( src, contours,
                      CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );
    ContourSet smallContours = getSmallContours( contours );
    f.mat.copyTo( clean.mat );
    cv::drawContours( clean.mat, smallContours, -1, cv::Scalar( 0 ), -1 );
    return clean;
}

FrameSet removeSmallConnectedComponents( FrameSet &fs ) {
    FrameSet result; result.reserve( fs.size() );
    std::transform( fs.begin(), fs.end(), std::back_inserter( result ),
                    std::ptr_fun( removeComponentsFromFrame ) );
    return result;
}
