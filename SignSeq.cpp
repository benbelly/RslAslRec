
#include "SignSeq.h"

SignSeq::SignSeq() {
}

SignSeq::~SignSeq() {
}

void SignSeq::AddHands( cv::Point tl, cv::Point br,
                        const cv::Mat &dom, const cv::Mat &weak ) {
    hands.push_back( FeatureFrame { tl, br, dom, weak,
                                    generateHandHistogram( dom.size(), getBoundary( dom ) ),
                                    weak.empty() ? Histogram() :
                                    generateHandHistogram( weak.size(), getBoundary( weak ) ) } );
}
