
#ifndef TRAININGDATA_H__
#define TRAININGDATA_H__

#include "Gloss.h"
#include "frame.h"
#include "histograms.h"
#include <vector>
#include <list>
#include <string>
#include <boost/shared_ptr.hpp>

class TrainingData {
    public:
        TrainingData( const std::vector<boost::shared_ptr<Gloss> > &glosses );
         cv::PCA &GetPCA() ;
         cv::Mat &GetCovariance( std::string word ) ;
    private:
        std::vector<boost::shared_ptr<Gloss> > glosses;

        std::list<Histogram> allHists;
        std::list<Histogram> GetHists() ;

        std::map<std::string, std::list<Histogram> > wordHists;
        std::map<std::string, std::list<Histogram> > GetWordHists();

        cv::Mat MakeBigVector( const std::list<Histogram> &hists );

        cv::PCA pca;
        cv::PCA MakePCA() ;

        std::map<std::string, cv::Mat> covar;
        std::map<std::string, cv::Mat> MakeCovar() ;
};
#endif
