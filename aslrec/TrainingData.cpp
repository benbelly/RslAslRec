
#include "frame.h"
#include "TrainingData.h"
#include "histograms.h"
#include "consts.h"
#include "writer.h"
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

cv::Mat &TrainingData::GetCovariance()  {
    return covar;
}

std::list<Histogram> TrainingData::GetHists()  {
    std::list<Histogram> hists;
    std::for_each( glosses.begin(), glosses.end(),
                   boost::bind( &Gloss::AppendHistograms, _1, &hists ) );
    if( hists.empty() )
        throw std::string( "No Histograms in TrainingData::GetHists()" );
    /*
     *std::vector<Histogram> vh;
     *std::copy( hists.begin(), hists.end(), std::back_inserter( vh ) );
     *writeMats( "cvsl_out/hist", vh );
     */
    return hists;
}

std::map<std::string, std::list<Histogram> > TrainingData::GetWordHists() {
    std::map<std::string, std::list<Histogram> > wordHists;
    std::vector<boost::shared_ptr<Gloss> >::iterator
                            begin = glosses.begin(),
                            end = glosses.end();
    while( begin != end ) {
        boost::shared_ptr<Gloss> glossPtr(*begin);
        wordHists[glossPtr->gloss] = std::list<Histogram>();
        std::vector<boost::shared_ptr<SignSeq> > seqs =
                                                glossPtr->Sequences();
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
    for( int i = 0; i < rows; ++i, ++iter ) {
        memcpy( big[i], iter->data, h.rows * h.cols * sizeof(double) );
        big[i];
    }
    return big;
}

cv::PCA TrainingData::MakePCA()  {
    // maxComponents is defined in consts.h
    return cv::PCA( MakeBigVector( allHists ), cv::Mat(),
                    CV_PCA_DATA_AS_ROW, maxComponents );
}

cv::Mat TrainingData::MakeCovar()  {
    const std::list<Histogram> &hists = allHists;
    std::list<Histogram> flats;
    std::transform( hists.begin(), hists.end(),
                    std::back_inserter( flats ),
                    boost::bind( &flattenHistogram, _1 ) );
    std::list<Histogram> projections;
    std::transform( flats.begin(), flats.end(),
                    std::back_inserter( projections ),
                    boost::bind( &cv::PCA::project, &pca, _1 ) );
    cv::Mat cov, inv, mean;
    cv::calcCovarMatrix( MakeBigVector( projections ), cov, mean,
                         CV_COVAR_NORMAL | CV_COVAR_ROWS );
    for( int i = 0; i < cov.rows; ++i )
        for( int j = 0; j < cov.cols; ++j )
            if( i != j ) cov.at<double>( i, j ) = 0.0;
    cv::invert( cov, inv, cv::DECOMP_SVD );

    return inv;
}
