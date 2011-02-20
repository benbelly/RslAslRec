
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

double FeatureFrame::distance( const Histogram testDom,
                               const cv::PCA &pca, const cv::Mat &covar ) {
    if( weakHist.get() ) { // two-handed training sign - not enough hands
        std::cerr << "Not enough hands penalty assigned\n";
        return NOT_ENOUGH_HANDS;
    }
    cv::Mat testDomProjection = pca.project( testDom );
    return cv::Mahalonobis( testDomProjection, GetDomProjection( pca ), covar );
}

double FeatureFrame::distance( const Histogram testDom,
                               const Histogram testWeak,
                               const cv::PCA &pca, const cv::Mat &covar ) {
    cv::Mat testDomProjection = pca.project( testDom ),
            trainDomProjection = GetDomProjection( pca );
    double distance = cv::Mahalonobis( testDomProjection, trainDomProjection, covar );

    if( weakHist.get() ) {
        cv::Mat testProj = pca.project( testWeak );
        distance += cv::Mahalonobis( testProj, GetWeakProjection( pca ), covar );
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
