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


#ifndef TRAINDB_H__
#define TRAINDB_H__

#include "Gloss.h"
#include "SignSeq.h"
#include "FeatureFrame.h"
#include "boost/shared_ptr.hpp"
#include "TrainingData.h"

#include<map>
#include<string>
#include<utility>
#include<vector>

class TrainDB {
    public:
        TrainDB();
        virtual ~TrainDB();

        cv::PCA Pca() const { return Data().GetPCA(); }
        cv::Mat Covariance() const { return Data().GetCovariance(); }
        double Distance( std::string word, int start, int end );

        int NumGlosses() const { return trainedSigns.size(); }

        std::string GetGloss( int i ) const { return glosses()[i]; }

        SignSeq *NextSequenceForGloss( std::string );

        std::vector<boost::shared_ptr<SignSeq> > Sequences( std::string gloss )
            { return trainedSigns[gloss]->Sequences(); }

        int AddHandToList( boost::shared_ptr<FeatureFrame> frame );

        boost::shared_ptr<FeatureFrame> GetFeatureFrame( int id )
            { return features[id]; }

        std::vector<int> ids();

        TrainingData &Data() const
        { if(!data.get()) data.reset(new TrainingData( GlossPtrs() ) );
          return *data.get(); }

    private:
        typedef boost::shared_ptr<Gloss> GlossPtr;
        typedef std::map<std::string, GlossPtr> GlossMap;
        GlossMap trainedSigns;

        mutable std::vector<std::string> glossesVec; // lazy initialization
        std::vector<std::string> glosses() const;
        std::vector<GlossPtr> GlossPtrs() const;

        std::map<int, boost::shared_ptr<FeatureFrame> > features;

        GlossMap::iterator GlossIter( std::string );

        mutable std::auto_ptr<TrainingData> data;
};

#endif
