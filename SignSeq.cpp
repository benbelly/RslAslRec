
#include "SignSeq.h"
#include "Databases.h"

SignSeq::SignSeq() {
}

SignSeq::~SignSeq() {
}

int SignSeq::AddHands( cv::Point tl, cv::Point br,
                        const cv::Mat &dom, const cv::Mat &weak ) {
    FeatureFrame frame { tl, br, dom, weak,
                         generateHandHistogram( dom.size(), getBoundary( dom ) ),
                         weak.empty() ? Histogram() :
                                        generateHandHistogram( weak.size(),
                                                               getBoundary( weak ) ) };
    std::shared_ptr<FeatureFrame> framePtr( new FeatureFrame( frame ) );
    hands.push_back( std::shared_ptr<FeatureFrame>( framePtr ) );
    return TDB->AddHandToList( framePtr );
}

double SignSeq::Distance( std::pair<int, int> interval ) {
    return 0.0;
}
