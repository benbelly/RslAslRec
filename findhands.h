
#ifndef FINDHANDS_H__
#define FINDHANDS_H__

// ML Stuff
#include "export.h"
#include "ml-types.h"

// API for sml
extern "C" {
    int numFrames( int type );
    void getFrameIds( int type, Pointer ids );

    void getFrameInfoC( int type, Pointer width, Pointer height,
                                 Pointer dtype, Pointer size );
    void getVideoInfoC( int type, Pointer color, Pointer fourcc, Pointer fps );
    void getFrame( int id, int type, Pointer img );

}

namespace frame_types {
    enum {
        original = 0,
        gray = 1,
        skin = 2,
        sd = 3,
        boundary = 4,
        histogram = 5,
        special = 6
    };
}

extern "C" {
  void InitAslAlg( char *cfile, int filenameLen );
  void findHands();
  /* This is probably 460 x 290 */
  int addHandImage( int width, int height,
		    int h1NumPts, Pointer h1Pts,
		    int h2NumPts, Pointer h2Pts );
}

#endif
