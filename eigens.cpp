
#include "eigens.h"
#include "consts.h"
#include <functional>

typedef cv::Mat_<double> PCAVec;

PCAVec makeBigVector( HistogramSet &ss ) {
    unsigned int vsize = ss[0].rows * ss[0].cols;
    PCAVec vec = PCAVec::zeros( vsize, ss.size() );
    // Copy each incoming histogram into its own row
    for( unsigned int i = 0; i < ss.size(); ++i )
        memcpy( vec[i], &ss[i], vsize * sizeof(double) );
    return vec;
}


cv::PCA pcaForHist( Histogram h ) {
    HistogramSet src( 1, h );
    PCAVec vec = makeBigVector( src );
    return cv::PCA( vec, cv::Mat(), CV_PCA_DATA_AS_ROW, maxComponents );
}

PcaSet pcaForHists( HistogramSet &hs ) {
    PcaSet pcas; pcas.reserve( hs.size() );
    std::transform( hs.begin(), hs.end(), std::back_inserter( pcas ),
                    std::ptr_fun( pcaForHist ) );
    return pcas;
}

cv::PCA pcaForTraining( HistogramSet hs ) {
    PCAVec vec = makeBigVector( hs );
    return cv::PCA( vec, cv::Mat(), CV_PCA_DATA_AS_ROW, maxComponents );
}
