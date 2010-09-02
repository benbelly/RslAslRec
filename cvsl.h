
#ifndef CVSL_H__
#define CVSL_H__

#include "export.h" // mlton types
#include "ml-types.h" // mlton types, for real

extern "C" {
    void showImageC( Pointer img, int width, int height, int type, Bool wait );
    void saveImageC( Pointer img, int width, int height, int type, char *fname, int fnameLen);

    void videoSaveBeginC( int width, int height, int fourcc, double fps, Bool color,
                         Pointer fname, int fnameLen );
    void saveVideoC( Pointer img, int width, int height, int type );
    void videoSaveEndC();
}

#endif
