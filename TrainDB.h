
#ifndef TRAINDB_H__
#define TRAINDB_H__

#include "Gloss.h"
#include "SignSeq.h"
#include "FeatureFrame.h"

#include<map>
#include<string>
#include<utility>
#include<vector>
#include<memory>

class TrainDB {
    public:
        TrainDB();
        virtual ~TrainDB();

        SignSeq *NextSequenceForGloss( std::string );

        int AddHandToList( std::shared_ptr<FeatureFrame> frame );
        std::shared_ptr<FeatureFrame> GetFeatureFrame( int id ) { return features[id]; }
        std::vector<int> ids();

    private:
        typedef std::shared_ptr<Gloss> SignPtr;
        typedef std::map<std::string, SignPtr> SignMap;
        SignMap trainedSigns;

        std::map<int, std::shared_ptr<FeatureFrame> > features;

        SignMap::iterator GlossIter( std::string );
};

extern TrainDB *TDB;

#endif
