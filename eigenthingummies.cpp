
#include "eigenthingummies.h"

typedef cv::Mat_<double> PCAVec;

static const int maxComponents = 4;

PCAVec makeBigVector( SampleSet &ss ) {
    unsigned int vsize = ss[0].rows * ss[0].cols;
    PCAVec vec = PCAVec::zeros( vsize, ss.size() );
    // Copy each incoming histogram into its own row
    for( unsigned int i = 0; i < ss.size(); ++i )
        memcpy( vec[i], &ss[i], vsize * sizeof(double) );
    return vec;
}

cv::PCA principalComponentAnalysis( SampleSet ss ) {
    if( ss.size() == 0 ) return cv::PCA();
    PCAVec vec = makeBigVector( ss );
    return cv::PCA( vec, cv::Mat(), CV_PCA_DATA_AS_ROW, maxComponents );
}
