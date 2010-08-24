
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
#include "utility.h"
#include "logging.h"

#include "FrameDB.h"

using std::string;
using std::cerr;
using std::endl;

// API for sml
    // Get a count of frames
/*
 *extern "C" int loadFrames( char *cfile, int filenameLen );
 *extern "C" void getFrames( int *frameBuf );
 */

int main( int, char **argv ) {
    string filename = argv[1];

    FrameDB db( filename );
    db.findHands();
    FrameSet sds = db.sds();
    std::for_each( sds.begin(), sds.end(), std::bind1st( std::ptr_fun( showNwait ), "sds" ) );

    return 0;
}

