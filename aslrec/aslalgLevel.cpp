
#include "Databases.h"
#include "boost/shared_ptr.hpp"
#include "boost/mem_fn.hpp"
#include<utility>
#include<string>
#include<limits>
#include<iostream>
#include "aslalgLevel.h" // This must go last - ml-types.h defines macros that cause trouble

using std::cout;
using std::endl;

int numberOfSequencesC( Pointer glossPtr, int glossLen ) {
    std::string gloss( (char *)glossPtr, glossLen );
    std::vector<boost::shared_ptr<SignSeq> > seqShared = TDB->Sequences( gloss );
    return seqShared.size();
}

void getSequencesC( Pointer glossPtr, int glossLen, Pointer sequencePtrs ) {
    std::string gloss( (char *)glossPtr, glossLen );
    std::vector<boost::shared_ptr<SignSeq> > seqShared = TDB->Sequences( gloss );
    std::transform( seqShared.begin(), seqShared.end(),
                    (SignSeq **)sequencePtrs,
                    boost::mem_fn( &boost::shared_ptr<SignSeq>::get ) );
}

double getMaxScoreC() { return std::numeric_limits<double>::max(); }

int getNumberOfSignsC() { return TDB->NumGlosses(); }

int getSignLengthC( int i ) { return TDB->GetGloss( i ).size(); }

void getSignC( int i, Pointer dst ) {
    std::string sign( TDB->GetGloss( i ) );
    const char *cs = sign.c_str();
    memcpy( dst, cs, sign.size() ) ;
}

typedef std::pair<int, int> Interval;
typedef std::pair<string, Interval> Key;
typedef std::map<Key, double> DistanceMemoMap;

double distanceC( Pointer word, int wordLen, int start, int end ) {
    std::string gloss( (char *)word, wordLen );
    
    // Memoize results or this will take F.O.R.E.V.E.R.
    static DistanceMemoMap &memos = *(new DistanceMemoMap());
    Key k = std::make_pair( gloss, std::make_pair( start, end ) );
    double distance = std::numeric_limits<double>::max();
    DistanceMemoMap::iterator i = memos.find( k );
    if( i == memos.end() ) {
        distance = TDB->Distance( gloss, start, end );
        memos[k] = distance;
    }
    else {
        distance = memos[k];
    }
    return distance;
}
