
#include "logging.h"
#include "FrameDB.h"

#include <string>

using std::string;

// API for sml
    // Get a count of frames
extern "C" {
    void InitSarkur( char *cfile, int filenameLen );
    void findHands();
}
//extern "C" void getFrames( int *frameBuf );


void InitSarkur( char *cfile, int filenameLen ) {
    new FrameDB( std::string( cfile, filenameLen ) );
}

void findHands() {
    FDB->findHands();
    FrameSet sds = FDB->sds();
    std::for_each( sds.begin(), sds.end(),
                   std::bind1st( std::ptr_fun( showNwait ), "sds" ) );
}



/*
 *
 *int main( int, char **argv ) {
 *    string filename = argv[1];
 *
 *    FrameDB db( filename );
 *    db.findHands();
 *    FrameSet sds = db.sds();
 *    std::for_each( sds.begin(), sds.end(), std::bind1st( std::ptr_fun( showNwait ), "sds" ) );
 *
 *    return 0;
 *}
 *
 */
