
#include "FeatureFrame.h"
#include "edgedetection.h"
#include "consts.h"
#include <iostream>

double myMahalanobis( const cv::Mat &l, const cv::Mat &r, const cv::Mat &icovar );

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
    else { // Single-hand sign penalty
        //std::cerr << "Single hand penalty assigned\n";
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

template<typename T>
    struct ArrHolder {
        ArrHolder(int size) : t( new T[size] ) { }
        ~ArrHolder() { delete[] t; }
        T *t;
    };
// Like My Sharona but mathier
double myMahalanobis( const cv::Mat &l, const cv::Mat &r, const cv::Mat &icovar ) {
    int len = l.rows * l.cols;
    const double *lp = (const double*)l.data,
                 *rp = (const double*)r.data,
                 *cov = (const double*)icovar.data;
    int covStep = icovar.step / sizeof(cov[0]);

    ArrHolder<double> arr(7);
    double *diff = arr.t;
    for( int i = 0; i < l.cols; ++i )
        diff[i] = lp[i] - rp[i];

    double result = 0;
    for( int i = 0; i < len; ++i, cov += covStep ) {
        double row_sum = 0;
        int j;
        for( j = 0; j <= len - 4; j += 4 )
            row_sum += diff[j]*cov[j] +
                       diff[j + 1] * cov[j + 1] +
                       diff[j + 2] * cov[j + 2] +
                       diff[j + 3] * cov[j + 3];
        for( ; j < len; ++j )
            row_sum += diff[j] * cov[j];
        result += row_sum * diff[i];
    }
    return std::sqrt( result );
}
