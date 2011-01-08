
#include<utility>
#include<algorithm>
#include<functional>
#include<limits>
#include "Gloss.h"
#include "eigens.h"
#include "boost/bind.hpp"
#include "boost/mem_fn.hpp"

Gloss::Gloss( std::string g ) : gloss( g ) {
}

Gloss::~Gloss() {}

SignSeq *Gloss::NextSeq() {
    boost::shared_ptr<SignSeq> ssp( new SignSeq() );
    trainedSeqs.push_back( ssp );
    return ssp.get();
}

std::vector<double> Gloss::GetScores( std::pair<int, int> interval ) {
    std::vector<double> distances; distances.reserve( trainedSeqs.size() );
    std::transform( trainedSeqs.begin(), trainedSeqs.end(),
                    std::back_inserter( distances ),
                    boost::bind( &SignSeq::Distance, _1, interval, GetICovarMatrix() ) );
    return distances;
}

double Gloss::Distance( int start, int end ) {
    std::vector<double> distances = GetScores( std::make_pair( start, end ) );
    std::vector<double>::iterator min = std::min_element( distances.begin(), distances.end() );
    if( min == distances.end() )
        return std::numeric_limits<double>::max();
    return *min;
}

const cv::Mat &Gloss::GetICovarMatrix() {
    if( icovarianceMatrix.empty() ) CalcICovarianceMatrix();
    return icovarianceMatrix;
}

void Gloss::CalcICovarianceMatrix() {
    // Get histograms from all frames of all videos of this gloss
    std::vector<cv::Mat> eigenValues;
    std::for_each( trainedSeqs.begin(), trainedSeqs.end(),
                   boost::bind( &SignSeq::CollectEigenValues, _1, eigenValues ) );
    cv::Mat covariance, mean;
    cv::calcCovarMatrix( &eigenValues[0], eigenValues.size(), covariance, mean,
                         CV_COVAR_NORMAL );
    cv::invert( covariance, icovarianceMatrix );
}

