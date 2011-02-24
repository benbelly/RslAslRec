
#include "FeatureFrame.h"
#include "edgedetection.h"
#include "consts.h"
#include <iostream>

FeatureFrame::FeatureFrame( cv::Point tl, cv::Point br, cv::Mat d, cv::Mat w ) :
    faceTopLeft( tl ), faceBottomRight( br ),
    dom( d ), weak( w ),
    domHist( generateHandHistogram( dom.size(), getBoundary( dom ) ) ),
    weakHist( 0 )
{
    if( weak.empty() == false ) {
        Histogram *hPtr = new Histogram(generateHandHistogram( weak.size(),
                                                               getBoundary( weak ) ) );
        weakHist.reset( hPtr );
    }

}

double FeatureFrame::distance( const cv::Mat &testDom,
                               const cv::PCA &pca, const cv::Mat &covar ) {
    if( weakHist.get() ) { // two-handed training sign - not enough hands
        return NOT_ENOUGH_HANDS;
    }
    return cv::Mahalonobis( testDom, GetDomProjection( pca ), covar );
}

double FeatureFrame::distance( const cv::Mat &testDom,
                               const cv::Mat &testWeak,
                               const cv::PCA &pca, const cv::Mat &covar ) {
    double distance = cv::Mahalonobis( testDom, GetDomProjection( pca ), covar );

    if( weakHist.get() ) {
        distance += cv::Mahalonobis( testWeak, GetWeakProjection( pca ), covar );
    }
    else { // Single-hand sign penalty
        std::cerr << "Single hand penalty assigned\n";
        distance += SINGLE_HAND_PENALTY;
    }
    return distance;
}

const cv::Mat &FeatureFrame::GetDomProjection( const cv::PCA &pca ) {
    if( !domProject.get() ) {
        domProject.reset( new cv::Mat( pca.project( domHist) ) );
    }
    return *(domProject.get());
}

const cv::Mat &FeatureFrame::GetWeakProjection( const cv::PCA &pca ) {
    if( !weakProject.get() ) {
        weakProject.reset( new cv::Mat( pca.project( *(weakHist.get()) ) ) );
    }
    return *(weakProject.get());
}
