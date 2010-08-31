
#include "logging.h"
#include "FrameDB.h"

#include <string>

// ML Stuff
#include "export.h"
#include "ml-types.h"

using std::string;

// API for sml
extern "C" {
    void InitSarkur( char *cfile, int filenameLen );
    void findHands();
    int numFrames();
    void getFrameIds( int *ids );

    void getFrameInfo( int type, Pointer width, Pointer height,
                                 Pointer dtype, Pointer size );
    void getFrame( int id, int type, char *img );
}

namespace frame_types {
    enum {
        original = 0,
        gray = 1,
        skin = 2,
        sd = 3,
        key = 4
    };
}
// API end


void InitSarkur( char *cfile, int filenameLen ) {
    new FrameDB( std::string( cfile, filenameLen ) );
}

void findHands() {
    FDB->findHands();
    FrameSet sds = FDB->sds();
    /*
     *std::for_each( sds.begin(), sds.end(),
     *               std::bind1st( std::ptr_fun( showNwait ), "sds" ) );
     */
}

int numFrames() { return FDB->size(); }

void getFrameIds( int *ids ) {
    std::vector<int> dbIds( FDB->ids() );
    std::copy( dbIds.begin(), dbIds.end(), ids );
}

int getFrameSize( const Frame &f ) {
    return f.size().height * f.size().width * f.mat.elemSize();
}

void getFrameInfo( int type, Pointer width, Pointer height,
                             Pointer dtype, Pointer size ) {
    Frame toCheck;
    switch( type ) {
        case frame_types::original:
            toCheck = FDB->original( 0 );
            break;
        default:
            toCheck = FDB->gray( 0 );
            break;
    }
    int *pw = (int*)width, *ph = (int*)height,
        *pt = (int*)dtype, *ps = (int*)size;
    *pw = toCheck.size().width;
    *ph = toCheck.size().height;
    *pt = toCheck.type();
    *ps = getFrameSize( toCheck );
}

Frame getFrame( int id, int type ) {
    switch( type ) {
        case frame_types::original:
            return FDB->original( id );
        case frame_types::gray:
            return FDB->gray( id );
        case frame_types::skin:
            return FDB->skin( id );
        case frame_types::sd:
            return FDB->sd( id );
        case frame_types::key:
            return FDB->key( id );
    }
    return Frame();
}

void getFrame( int id, int type, char *img ) {
    Frame get = getFrame( id, type );
    std::copy( get.mat.datastart, get.mat.dataend, img );
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
