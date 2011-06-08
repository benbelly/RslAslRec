
#include "FeatureFrame.h"
#include "edgedetection.h"
#include "consts.h"
#include <iostream>

FeatureFrame::FeatureFrame( cv::Point tl, cv::Point br, Contour d,
                            boost::shared_ptr<Contour> w ) :
    faceTopLeft( tl ), faceBottomRight( br ),
    dom( d ), weak( w ),
    domHist( generateHandHistogram( dom, weak.get() ) )
{
}

double FeatureFrame::distance( const cv::Mat &testHands,
                               const cv::PCA &pca, const cv::Mat &covar ) {
    return cv::Mahalanobis( testHands, GetDomProjection( pca ), covar );
}

const cv::Mat &FeatureFrame::GetDomProjection( const cv::PCA &pca ) {
    if( !projection.get() ) {
        projection.reset( new cv::Mat( pca.project( flattenHistogram( domHist ) ) ) );
    }
    return *(projection.get());
}

