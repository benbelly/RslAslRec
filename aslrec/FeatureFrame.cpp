
#include "FeatureFrame.h"
#include "edgedetection.h"
#include "consts.h"
#include <iostream>

double ALPHA_SCALE_FACTOR = 0.4;
double SINGLE_HAND_PENALTY = 0.04;

FeatureFrame::FeatureFrame( cv::Point tl, cv::Point br, cv::Mat d,
                            boost::shared_ptr<cv::Mat> w ) :
    faceTopLeft( tl ), faceBottomRight( br ),
    dom( d ), weak( w ),
    domHist( generateHandHistogram( dom.size(), getBoundary( dom ) ) ),
    weakHist( 0 )
{
    if( weak.get() ) {
        Histogram *hPtr = new Histogram(generateHandHistogram( weak->size(),
                                                               getBoundary( *(weak.get()) ) ) );
        weakHist.reset( hPtr );
    }

}

double FeatureFrame::distance( const cv::Mat &testDom,
                               const cv::PCA &pca, const cv::Mat &covar ) {
    if( weakHist.get() ) { // two-handed training sign - not enough hands
        return NOT_ENOUGH_HANDS;
    }
    return cv::Mahalanobis( testDom, GetDomProjection( pca ), covar );
}

double FeatureFrame::distance( const cv::Mat &testDom,
                               const cv::Mat &testWeak,
                               const cv::PCA &pca, const cv::Mat &covar ) {
    double distance = cv::Mahalanobis( testDom, GetDomProjection( pca ), covar );

    if( weakHist.get() ) {
        distance += cv::Mahalanobis( testWeak, GetWeakProjection( pca ), covar );
    }
    else {
        distance += SINGLE_HAND_PENALTY;
    }
    return distance;
}

const cv::Mat &FeatureFrame::GetDomProjection( const cv::PCA &pca ) {
    if( !domProject.get() ) {
        domProject.reset( new cv::Mat( pca.project( flattenHistogram( domHist ) ) ) );
    }
    return *(domProject.get());
}

const cv::Mat &FeatureFrame::GetWeakProjection( const cv::PCA &pca ) {
    if( !weakProject.get() ) {
        weakProject.reset( new cv::Mat( pca.project( flattenHistogram( *(weakHist.get()) ) ) ) );
    }
    return *(weakProject.get());
}

