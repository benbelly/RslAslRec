
#include "frame.h"
#include "histograms.h"
#include "consts.h"
#include "TrainingData.h"
#include <boost/bind.hpp>

TrainingData::TrainingData( const std::vector<boost::shared_ptr<Gloss> > &gs ) :
    glosses( gs ),
    allHists( GetHists() ),
    wordHists( GetWordHists() ),
    pca( MakePCA() ),
    covar( MakeCovar() )
{
}

 cv::PCA &TrainingData::GetPCA()  {
    return pca;
}

 cv::Mat &TrainingData::GetCovariance( std::string word )  {
    return covar[word];
}

std::list<Histogram> TrainingData::GetHists()  {
    std::list<Histogram> hists;
    std::for_each( glosses.begin(), glosses.end(),
                   boost::bind( &Gloss::AppendHistograms, _1, &hists ) );
    if( hists.empty() ) throw std::string( "No Histograms in TrainingData::GetHists()" );
    return hists;
}

std::map<std::string, std::list<Histogram> > TrainingData::GetWordHists() {
    std::map<std::string, std::list<Histogram> > wordHists;
    std::vector<boost::shared_ptr<Gloss> >::iterator begin = glosses.begin(),
                                                     end = glosses.end();
    while( begin != end ) {
        boost::shared_ptr<Gloss> glossPtr(*begin);
        wordHists[glossPtr->gloss] = std::list<Histogram>();
        std::vector<boost::shared_ptr<SignSeq> > seqs = glossPtr->Sequences();
        std::for_each( seqs.begin(), seqs.end(),
                       boost::bind( &SignSeq::AppendHistograms, _1,
                                    &(wordHists[glossPtr->gloss]) ) );
        ++begin;
    }
    return wordHists;
}

cv::Mat TrainingData::MakeBigVector( const std::list<Histogram> &hists ) {
    Histogram h = *(hists.begin());
    // Make a giant matrix where each histogram is a row
    int rows = hists.size();
    Histogram big = Histogram::zeros( rows, h.rows * h.cols );
    std::list<Histogram>::const_iterator iter = hists.begin();
    for( int i = 0; i < rows; ++i, ++iter )
        memcpy( big[i], iter->data, h.rows * h.cols * sizeof(double) );
    return big;
}

cv::PCA TrainingData::MakePCA()  {
    return cv::PCA( MakeBigVector( allHists ), cv::Mat(), CV_PCA_DATA_AS_ROW, maxComponents );
}

std::map<std::string, cv::Mat> TrainingData::MakeCovar()  {
    std::map<std::string, cv::Mat> covariants;
    std::map<std::string, std::list<Histogram> >::iterator begin = wordHists.begin(),
                                                           end = wordHists.end();
    while( begin != end ) {
        const std::string &word = begin->first;
        const std::list<Histogram> &hists = begin->second;
        Histogram src = *(hists.begin());
        std::list<Histogram> flats;
        std::transform( hists.begin(), hists.end(),
                        std::back_inserter( flats ),
                        boost::bind( &flattenHistogram, _1 ) );
        Histogram flat = *(flats.begin());
        std::list<Histogram> projections;
        std::transform( flats.begin(), flats.end(),
                        std::back_inserter( projections ),
                        boost::bind( &cv::PCA::project, &pca, _1 ) );
        Histogram proj = *(projections.begin());
        cv::Mat cov, inv, mean;
        cv::calcCovarMatrix( MakeBigVector( projections ), cov, mean,
                             CV_COVAR_NORMAL | CV_COVAR_ROWS );
        for( int i = 0; i < cov.rows; ++i )
            for( int j = 0; j < cov.cols; ++j )
                if( i != j ) cov.at<double>( i, j ) = 0.0;
        cv::invert( cov, inv, cv::DECOMP_SVD );
        covariants[word] = inv;
        ++begin;
    }
    return covariants;
}
