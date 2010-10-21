
#include <vector>
#include <list>
#include <utility>
#include <limits>
#include <algorithm>
#include <functional>
#include "frame.h"

// TODO: Grammar checking - sign combinations that are not allowed in the grammer have a
// constant cost. The distance to those 'illegal' signs should not be calculated

typedef cv::PCA SignFeatureVec;

struct ModelSign {
    std::string gloss;
    SignFeatureVec features;
};

typedef std::vector<ModelSign> ModelBase;

double distance( ModelSign sign, int startFrame, int endFrame ) {
    // Calculate the distance of frame sequence start->end
    // from the sign
    // TODO: Memoize the sign/Framepair pair for performance
    // TODO: Memoize the startFrame/endFrame pair for performance
    // TODO: Oh, and implement distance.  :)
    // TODO: ME, of course!!
    return 0.0;
}

struct Label {
    Label( ModelSign s ) : sign( s ), endFrame( -1 ),
                           distance( std::numeric_limits<double>::max() ) { };
    ModelSign sign;
    int endFrame;
    double distance;
};

// Cheap use of pair to allow std algorithms
Label MforSign( ModelSign sign, std::pair<int, int> startAndEnd ) {
    int startFrame = startAndEnd.first, end = startAndEnd.second;
    Label signScore( sign );
    for( int m = startFrame + 1; m < end; ++m ) {
        double dist = distance( sign, startFrame, m );
        if( dist < signScore.distance ) {
            signScore.distance = dist;
            signScore.endFrame = m;
        }
    }
    return signScore;
}

//
// Returns a collection of best-end-frames for every sign in the ModelBase
//
std::vector<Label> ScoresForSigns( ModelBase signDB,
                                   int startFrame, int size ) {
    std::vector<Label> scores; scores.reserve( size );
    std::transform( signDB.begin(), signDB.end(),
                    std::back_inserter( scores ),
                    std::bind2nd( std::ptr_fun( MforSign ),
                                  std::pair<int, int>( startFrame, size ) ) );
    return scores;
}


// TODO: These all assume some sort of global current video
const static int videoSize = 141;
const static unsigned int maxLevels = 20;

typedef std::vector<Label> Sentence;

// TODO: Limit the number of sentences pursued?
std::vector<Sentence> SignsForLevels( ModelBase signDB ) {
    std::vector<Sentence> bestSentences; bestSentences.reserve( signDB.size() );
    // Level 0!
    std::vector<Label> firstLabels = ScoresForSigns( signDB, 0, videoSize );
    for( unsigned int lvl = 1; lvl < maxLevels; ++lvl ) {
    }
    return bestSentences;
}
