
#ifndef TRAINDB_H__
#define TRAINDB_H__

#include "Gloss.h"
#include "SignSeq.h"
#include "FeatureFrame.h"
#include "boost/shared_ptr.hpp"

#include<map>
#include<string>
#include<utility>
#include<vector>

class TrainDB {
    public:
        TrainDB();
        virtual ~TrainDB();

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

    private:
        typedef boost::shared_ptr<Gloss> GlossPtr;
        typedef std::map<std::string, GlossPtr> GlossMap;
        GlossMap trainedSigns;
        mutable std::vector<std::string> glossesVec; // lazy initialization
        std::vector<std::string> glosses() const;

        std::map<int, boost::shared_ptr<FeatureFrame> > features;

        GlossMap::iterator GlossIter( std::string );
};

#endif
