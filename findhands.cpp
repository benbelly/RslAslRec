
#include <vector>
#include <numeric>
#include <string>
#include <iterator>
#include <functional>
#include <algorithm>
#include <iostream>
#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "frame.h"
#include "keyframeselect.h"
#include "keyframedist.h"
#include "skinmask.h"
#include "edgedetection.h"
#include "logging.h"

using std::string;
using std::cerr;
using std::endl;

extern "C" int *getFrameIds( char *cfile, int filenameLen );

CsAndGs *allFrames;
std::vector<int> *frameIds;
cv::VideoCapture *vidCap;

CsAndGs *getFrames( cv::VideoCapture &cap ) {
    double numFrames = cap.get( CV_CAP_PROP_FRAME_COUNT );

    allFrames = new CsAndGs();
    frameIds = new std::vector<int>(); frameIds->reserve( numFrames );

    allFrames->colors.reserve( numFrames );
    allFrames->grays.reserve( numFrames );

    int i = 0;
    cv::Mat img;
    while( cap.grab() ) {
        cap.retrieve( img );
        Frame color( i, img.size(), img.type() );
        img.copyTo( color.mat );
        Frame gray( i, img.size(), CV_8UC1 );
        cv::cvtColor( img, (gray.mat), CV_RGB2GRAY );
        allFrames->colors.push_back( color );
        allFrames->grays.push_back( gray );
        frameIds->push_back( i++ );
    }
    //cerr << "Got " << frames.size() << " frames of expected " << numFrames << endl;
    return allFrames;
}

int *getFrameIds( char *cfile, int filenameLen ) {
    string filename( cfile, filenameLen );
    vidCap = new cv::VideoCapture( filename );
    getFrames( *vidCap );
    cout << "num FrameIds = " << frameIds->size() << endl;
    return &(*(frameIds->begin()));
}

FrameSet getKeyframes( FrameSet &frames ) {
    FrameSet keyframes;
    keyframes.reserve( frames.size() ); // can't get bigger than this

    keyframes.push_back( frames[0] );
    std::accumulate( frames.begin() + 1, frames.end(),
                     frames[0],
                     AccumKeyframes( keyframes ) );

    return keyframes;
}

FrameSet getSDs( const FrameSet &frames, const FrameSet &keyFrames ) {
    // I happen to know these get converted to 16 bit FRAMES times if I
    // don't do it ahead of time
    FrameSet bigKeys = gray8bitTogray16bit( keyFrames );
    FrameSet SDs;
    SDs.reserve( frames.size() );
    std::transform( frames.begin(), frames.end(), std::back_inserter( SDs ),
                    std::bind1st( std::ptr_fun( avgDist ), bigKeys ) );
    return SDs;
}

int main( int, char **argv ) {
    string filename = argv[1];
    cv::VideoCapture cap( filename );
    CsAndGs &allFrames = *getFrames( cap );

    FrameSet keyframes = getKeyframes( allFrames.grays );
    TRACECOUNT( "Keyframes", keyframes.size() );

    FrameSet SDs = getSDs( allFrames.grays, keyframes );
    //std::for_each( SDs.begin(), SDs.end(), std::bind1st( std::ptr_fun( showNwait ), "SDs" ) );

    SkinMaskSet skinMasks = generateSkinMasks( allFrames.colors );
    /*
     *std::for_each( skinMasks.begin(), skinMasks.end(),
     *               std::mem_fun_ref( &SkinMask::showNwait ) );
     */

    FrameSet skinMasked; skinMasked.reserve( SDs.size() );
    Zip( SDs.begin(), SDs.end(), skinMasks.begin(), skinMasks.end(),
         std::back_inserter( skinMasked ), maskFrame );
    /*
     *std::for_each( skinMasked.begin(), skinMasked.end(),
     *               std::bind1st( std::ptr_fun( showNwait ), "masked" ) );
     */

    FrameSet edges = getDilatedEdges( skinMasked );
    //FrameSet edges = getEdges( skinMasked );
    std::for_each( edges.begin(), edges.end(),
                   std::bind1st( std::ptr_fun( showNwait ), "edges" ) );

    FrameSet edgeMasked = negateAndMask( skinMasked, edges );
    /*
     *std::for_each( edgeMasked.begin(), edgeMasked.end(),
     *               std::bind1st( std::ptr_fun( showNwait ), "edges2" ) );
     */

    FrameSet smallsRemoved = removeSmallConnectedComponents( edgeMasked );
    /*
     *std::for_each( smallsRemoved.begin(), smallsRemoved.end(),
     *               std::bind1st( std::ptr_fun( showNwait ), "smalls removed" ) );
     */

    return 0;
}

