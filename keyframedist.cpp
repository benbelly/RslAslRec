#include <vector>
#include <numeric>
#include "frame.h"
#include "keyframedist.h"
#include "logging.h"
#include "consts.h"

struct FrameKeyFrameDiffSum {
    FrameKeyFrameDiffSum( const Frame &frame, cv::Mat &m ) : f( frame ),
                                                             diffSum( m )
    { }

    cv::Mat like() const {
        return cv::Mat( diffSum.size(), diffSum.type() );
    }
    Frame f;
    cv::Mat diffSum;
};

FrameKeyFrameDiffSum frameKeyFrameDiff( FrameKeyFrameDiffSum accum, Frame kf ) {
    cv::Mat &Si( accum.f.mat ),
            &kf16u( kf.mat );

    cv::Mat diff( accum.like() );
    absdiff( Si, kf16u, diff );

    cv::Mat sum( accum.like() );
    cv::add( accum.diffSum, diff, sum );

    accum.diffSum = sum;
    return accum;
}

cv::Mat make16bit( const cv::Mat &src ) {
    cv::Mat kf16u( src.size(), src.type() );
    src.convertTo( kf16u, CV_16U );
    return kf16u;
}

Frame avgDist( const FrameSet &keyFrames, const Frame f ) {
    cv::Mat Si = make16bit( f.mat ),
                 kf16u( keyFrames[1].mat ); // already 16bit

    cv::Mat diff( Si.size(), Si.type() );
    absdiff( Si, kf16u, diff );

    FrameKeyFrameDiffSum diffsum( Frame( f.id, Si ), kf16u );
    FrameKeyFrameDiffSum sum = std::accumulate( keyFrames.begin() + 1, keyFrames.end(),
                                                diffsum, frameKeyFrameDiff );

    double one = 1.0, mp = keyFrames.size() - 1;
    cv::Mat avg( cv::abs( sum.diffSum * ( one / mp ) ) );

    cv::Mat avg8b;
    avg.convertTo( avg8b, CV_8UC1 );

    return Frame( f.id, avg8b );
}

