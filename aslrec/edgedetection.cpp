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


#include "edgedetection.h"
#include "consts.h"
#include "utility.h"
#include "Databases.h"

static const int allContours = -1;

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
    cv::drawContours( clean.mat, smallContours, allContours, cv::Scalar( 0 ), -1 );
    return clean;
}

FrameSet removeSmallConnectedComponents( FrameSet &fs ) {
    FrameSet result; result.reserve( fs.size() );
    std::transform( fs.begin(), fs.end(), std::back_inserter( result ),
                    std::ptr_fun( removeComponentsFromFrame ) );
    return result;
}

bool compactAndSmall( Contour c ) {
    /*
     *All connected components that are compact
     *and small are selected to be hand candidates. The
     *compactness is measured by dividing the number of pixels
     *by the number of boundary pixels with a threshold T2 . The
     *size is measured by the number of pixels with threshold T3 .
     */
    double boundary = (double)c.size(), // contour acquired using CV_CHAIN_APPROX_NONE
           area = cv::contourArea( cv::Mat( c ) );
    return ( area < T3 ) && ( ( area / boundary ) > T2 );
}

ContourSet getCompactAndSmallContours( const cv::Mat &img ) {
    cv::Mat src = cv::Mat::zeros( img.size(), img.type() );
    img.copyTo( src );
    ContourSet contours;
    cv::findContours( src, contours,
                      CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );
    ContourSet csc;
    Copy_if( contours.begin(), contours.end(), std::back_inserter( csc ),
             std::ptr_fun( compactAndSmall ) );
    return csc;
}

ContourSet getContours( const cv::Mat &img ) {
    cv::Mat src = cv::Mat::zeros( img.size(), img.type() );
    img.copyTo( src );
    ContourSet contours;
    cv::findContours( src, contours,
                      CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );
    return contours;
}

std::pair<Frame, ContourSet> getBoundaryImage( Frame f ) {
    Frame clean( f.id, f.size(), f.type() );
    ContourSet contours( getCompactAndSmallContours( f.mat ) );
    cv::drawContours( clean.mat, contours, allContours, cv::Scalar( 255 ) );
    if( contours.empty() ) contours.push_back( Contour() );
    //clean.mat.row( clean.mat.rows / 2 ).col( clean.mat.cols / 2 ) = cv::Scalar( 255 );
    return std::pair<Frame, ContourSet>( clean, contours );
}

ContourSet getHands( const cv::Mat &img ) {
    cv::Mat dst( img.size(), img.type() );
    ContourSet contours( getContours( dst ) );
    return contours;
}

Contour getBoundary( const cv::Mat &src ) {
    cv::Mat img = cv::Mat::zeros( src.size(), src.type() );
    src.copyTo( img );
    ContourSet contours;
    cv::findContours( img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );
    Contour result = contours.empty() ? Contour() : *(contours.begin());
    return result;
}

FrameHandSet getBoundaryImages( FrameSet &fs ) {
    std::vector< std::pair<Frame, ContourSet> > result; result.reserve( fs.size() );
    std::transform( fs.begin(), fs.end(), std::back_inserter( result ),
                    std::ptr_fun( getBoundaryImage ) );
    return result;
}

CenterPoint center( Contour contour ) {
    if( contour.empty() ) return cv::Point( 0, 0 );
    cv::Mat dst( contour );
    CenterPoint center;
    float radius;
    cv::minEnclosingCircle( dst, center, radius );
    return center;
}

CenterSet centers( ContourSet &contours ) {
    std::vector<cv::Point2f> cs; cs.reserve( contours.size() );
    std::transform( contours.begin(), contours.end(),
                    std::back_inserter( cs ),
                    std::ptr_fun( center ) );
    return cs;
}
