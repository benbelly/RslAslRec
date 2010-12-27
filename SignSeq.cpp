
#include "SignSeq.h"

SignSeq::SignSeq() {
}

SignSeq::~SignSeq() {
}

void SignSeq::AddHands( const cv::Mat &dom, const cv::Mat &weak ) {
    hands.push_back( HandPair( dom, weak ) );
}
