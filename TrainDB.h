
#ifndef TRAINDB_H__
#define TRAINDB_H__

#include "Gloss.h"
#include "SignSeq.h"

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

    private:
        typedef std::shared_ptr<Gloss> SignPtr;
        typedef std::map<std::string, SignPtr> SignMap;
        SignMap trainedSigns;

        SignMap::iterator GlossIter( std::string );
};

extern TrainDB *TDB;

#endif
