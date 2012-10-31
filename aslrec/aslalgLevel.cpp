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

double distanceC( Pointer word, int wordLen, int start, int end ) {

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
