
#include "FeatureFrame.h"
#include "edgedetection.h"
#include "eigens.h"

FeatureFrame::FeatureFrame( cv::Point tl, cv::Point br, cv::Mat d, cv::Mat w ) :
    faceTopLeft( tl ), faceBottomRight( br ),
    dom( d ), weak( w ),
    domHist( generateHandHistogram( dom.size(), getBoundary( dom ) ) ),
    weakHist( weak.empty() ? Histogram() :
                             generateHandHistogram( weak.size(), getBoundary( weak ) ) ),
    domPCA( pcaForHist( domHist ) ),
    weakPCA( weak.empty() ? cv::PCA() : cv::PCA() )
{

}

double FeatureFrame::distance( Histogram testDom, Histogram /*testWeak*/, const cv::Mat &icovar ) {
    return cv::Mahalonobis( testDom, domHist, icovar );
}
