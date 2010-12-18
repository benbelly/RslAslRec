
#include <string>
#include <string.h>
#include <iostream>
#include <map>

#include "FrameDB.h"
#include "findhands.h"

using std::string;
using std::cerr;
using std::endl;

typedef std::map<int, cv::Mat> SpecialImageMap;
SpecialImageMap specialImages;

void InitAslAlg( char *cfile, int filenameLen ) {
    FrameSet vidFrames = loadFromVideo( std::string( cfile, filenameLen ) );
    new FrameDB( vidFrames );
}

void findHands() {
    FDB->findHands();
    FrameSet sds = FDB->sds();
}

int numFrames( int type ) {
    switch( type ) {
        case frame_types::special:
            return specialImages.size();
        default:
            return FDB->size();
    }
}

int toKey( SpecialImageMap::value_type p ) {
    return p.first;
}
void getFrameIds( int type, Pointer ids ) {
    switch( type ) {
        case frame_types::special:
        {
            std::vector<int> dbIds; dbIds.reserve( specialImages.size() );
            std::transform( specialImages.begin(), specialImages.end(),
                            std::back_inserter( dbIds ), std::ptr_fun( toKey ) );
            std::copy( dbIds.begin(), dbIds.end(), (int*)ids );
        }
        default:
        {
            std::vector<int> dbIds( FDB->ids() );
            std::copy( dbIds.begin(), dbIds.end(), (int*)ids );
        }
    }
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
        case frame_types::special: // Meaningless
            toCheck = Frame( 0, specialImages[0] );
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
        case frame_types::special:
            return Frame( id, specialImages[id] );
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
 * The input data skips scan lines and does weird things
 * This tries to correct for _most_ of it.
 */
void makeImageFromData( cv::Mat &img, int numPts, int *pts ) {
    int i = 0, x = pts[i++], y = pts[i++];
    // Since y jumps around, track the jump and increment actualY
    // Sometimes Y changes (incorrectly) in the middle of scanlines,
    // so track X position as well, and only increment y when both change
    int actualY = y, lastX, lastY = y;
    while( i < numPts ) {
        img.at<unsigned char>( actualY, x ) = 255;
        lastX = x;
        x = pts[i++]; y = pts[i++];
        if( y != lastY &&
            x != (lastX + 1) ) {
            lastY = y; ++actualY;
        }
    }
}

int addHandImage( int width, int height,
		  int h1NumPts, Pointer h1Pts,
		  int h2NumPts, Pointer h2Pts ) {
    cv::Mat hand = cv::Mat::zeros( height, width, image_types::gray );
    makeImageFromData( hand, h1NumPts, (int *)h1Pts );
    if( h2NumPts ) makeImageFromData( hand, h2NumPts, (int *)h2Pts );
    static int lastIndex = 0;
    specialImages[lastIndex] = hand;
    return lastIndex++;
}

