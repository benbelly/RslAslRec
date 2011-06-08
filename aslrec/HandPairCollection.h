
#ifndef _HandPairCollection_H__
#define _HandPairCollection_H__

#include "edgedetection.h"
#include "histograms.h"
#include "frame.h"
#include <boost/shared_ptr.hpp>

class HandPairCollection {
    public:
        typedef Projection HandPair;
        typedef std::pair<CenterPoint, boost::shared_ptr<CenterPoint> > CenterPair;

        HandPairCollection() {}
        HandPairCollection( const cv::PCA &pca, const ContourSet &hands );

        int size() const { return hands.size(); }
        HandPair pair( int i ) const { return hands[i]; }
        CenterPair center( int i ) const { return centers[i]; }

    private:
        std::vector<HandPair> hands;
        std::vector<CenterPair> centers;
        void makePairs( const cv::PCA &pca, const ContourSet &singles );
};
#endif
