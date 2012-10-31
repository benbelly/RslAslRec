/**
    Copyright 2011 Ben Holm

    This file is part of RslAslRec.

    RslAslRec is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RslAslRec is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RslAslRec.  If not, see <http://www.gnu.org/licenses/>.
**/


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
