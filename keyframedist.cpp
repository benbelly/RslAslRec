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
        return cv::Mat( cv::Mat::zeros( diffSum.size(), diffSum.type() ) );
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
    cv::Mat dst = cv::Mat::zeros( src.size(), image_types::big_gray );
    src.convertTo( dst, image_types::big_gray, 255 );
    /*
     *cv::imshow( "src", src );
     *cv::imshow( "dst", dst );
     *cv::waitKey();
     */
    return dst;
}

Frame avgDist( const FrameSet &keyFrames, const Frame f ) {
    cv::Mat Si( make16bit( f.mat ) ),
            firstKey( keyFrames[0].mat ); // already 16bit

    cv::Mat diff = cv::Mat::zeros( Si.size(), Si.type() );
    absdiff( Si, firstKey, diff );

    FrameKeyFrameDiffSum diffsum( Frame( f.id, Si ), firstKey );
    FrameKeyFrameDiffSum sum = std::accumulate( keyFrames.begin() + 1, keyFrames.end(),
                                                diffsum, frameKeyFrameDiff );

    double one = 1.0, mp = keyFrames.size() - 1;
    cv::Mat avg( cv::abs( sum.diffSum * ( one / mp ) ) );

    cv::Mat avg8b = cv::Mat::zeros( avg.size(), image_types::gray );
    avg.convertTo( avg8b, image_types::gray );

    return Frame( f.id, avg8b );
}

