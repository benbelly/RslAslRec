
#include "FeatureFrame.h"
#include "edgedetection.h"

FeatureFrame::FeatureFrame( cv::Point tl, cv::Point br, cv::Mat d, cv::Mat w ) :
    faceTopLeft( tl ), faceBottomRight( br ),
    dom( d ), weak( w ),
    domHist( generateHandHistogram( dom.size(), getBoundary( dom ) ) ),
    weakHist( weak.empty() ? Histogram() :
                             generateHandHistogram( weak.size(), getBoundary( weak ) ) )
{

}

double FeatureFrame::distance( Histogram testDom, Histogram testWeak ) {
    return 0.0;
}
