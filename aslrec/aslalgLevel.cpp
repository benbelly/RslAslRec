
#include "Databases.h"
#include "consts.h"
#include "boost/shared_ptr.hpp"
#include "boost/mem_fn.hpp"
#include "boost/filesystem.hpp"
#include<utility>
#include<string>
#include<limits>
#include<iostream>
#include<fstream>
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
typedef std::pair<std::string, Interval> Key;
typedef std::map<Key, double> DistanceMemoMap;
static DistanceMemoMap &memos = *(new DistanceMemoMap());

void loadScoresC( Pointer fname, int fnameLen ) {
    std::string infilename( (char *)fname, fnameLen );
    // If there is no file, warn and continue
    if( boost::filesystem::exists( infilename ) == false ) {
        std::cerr << "WARNING - No score file " << infilename << std::endl;
        return;
    }
    std::ifstream infile( infilename.c_str() );
    while( !infile.eof() ) {
        Key k;
        double score;
        infile >> k.first >> k.second.first >> k.second.second >> score;
        memos[k]=score;
    }
}

double distanceC( double alpha, Pointer word, int wordLen, int start, int end ) {

/*
 *static double ALPHA_SCALE_FACTOR = 0.4;
 *static double SINGLE_HAND_PENALTY = 0.04;
 */
    SINGLE_HAND_PENALTY = alpha * ALPHA_SCALE_FACTOR;
    SINGLE_HAND_PENALTY = 0.0;

    std::string gloss( (char *)word, wordLen );
    
    // Memoize results or this will take F.O.R.E.V.E.R.
    //static DistanceMemoMap &memos = *(new DistanceMemoMap());
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

void dumpScoresC( Pointer fname, int fnameLen ) {
    std::string outfileName( (char *)fname, fnameLen );
    std::ofstream out( outfileName.c_str() );
    DistanceMemoMap::iterator i = memos.begin(), end = memos.end();
    while( i != end ) {
        Key k = i->first;
        double score = i->second;
        out << k.first << " " << k.second.first << " "
            << k.second.second << " " << score << endl;
        ++i;
    }
}
