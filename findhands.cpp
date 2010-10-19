
#include <string>
#include <string.h>
#include <iostream>

#include "logging.h"
#include "FrameDB.h"
#include "findhands.h"

using std::string;
using std::cerr;
using std::endl;

void InitSarkur( char *cfile, int filenameLen ) {
    FrameSet vidFrames = loadFromVideo( std::string( cfile, filenameLen ) );
    new FrameDB( vidFrames );
}

void findHands() {
    FDB->findHands();
    FrameSet sds = FDB->sds();
}

int numFrames() { return FDB->size(); }

void getFrameIds( Pointer ids ) {
    std::vector<int> dbIds( FDB->ids() );
    std::copy( dbIds.begin(), dbIds.end(), (int*)ids );
}

int getFrameSize( const Frame &f ) {
    return f.size().height * f.size().width * f.mat.elemSize();
}

void getFrameInfoC( int type, Pointer width, Pointer height,
                    Pointer dtype, Pointer size ) {
    Frame toCheck;
    switch( type ) {
        case frame_types::original:
            toCheck = FDB->original( 0 );
            break;
        case frame_types::histogram:
            toCheck = FDB->histogram( 0 );
            break;
        default:
            toCheck = FDB->gray( 0 );
            break;
    }
    *((int*)width) = toCheck.size().width;
    *((int*)height) = toCheck.size().height;
    *((int*)dtype) = toCheck.type();
    *((int*)size) = getFrameSize( toCheck );
}

bool isColor( int type ) {
    switch( type ) {
        case frame_types::original:
            return FDB->original( 0 ).channels() >= 3;
            break;
        default:
            return false;
            break;
    }
}

void getVideoInfoC( int type, Pointer color, Pointer fourcc, Pointer fps ) {
    *((Bool*)color) = isColor( type );
    *((int*)fourcc) = FDB->getFourcc();
    *((double*)fps) = FDB->getFps();
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
        case frame_types::boundary:
            return FDB->boundary( id );
        case frame_types::histogram:
            return FDB->histogram( id );
    }
    return Frame();
}

void getFrame( int id, int type, Pointer img ) {
    Frame get = getFrame( id, type );
    if( get.mat.isContinuous() )
        memcpy( img, get.mat.datastart, get.mat.dataend - get.mat.datastart );
    else
        cerr << "HELP!" << endl;
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
