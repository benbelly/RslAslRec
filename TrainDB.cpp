
#include "TrainDB.h"

TrainDB *TDB;

TrainDB::TrainDB() {
    TDB = this;
}

TrainDB::~TrainDB() {
}

SignSeq *TrainDB::NextSequenceForGloss( std::string g ) {
    SignMap::iterator glossI = GlossIter( g );
    return (glossI->second)->NextSeq();
}

TrainDB::SignMap::iterator TrainDB::GlossIter( std::string g ) {
    SignMap::iterator forGloss = trainedSigns.find( g ),
                      end = trainedSigns.end();
    if( forGloss == end )
        forGloss = trainedSigns.insert( trainedSigns.begin(),
                                        std::make_pair( g, SignPtr( new Gloss( g ) ) ) );
    return forGloss;
}
