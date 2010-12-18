
#ifndef CVSL_H__
#define CVSL_H__

#include "export.h" // mlton types
#include "ml-types.h" // mlton types, for real

extern "C" {
    /*
     * Display an image with cv image type of 'type'. If wait is true, wait
     * for a key-press.
     */
    void showImageC( Pointer img, int width, int height, int type, Bool wait );
    /*
     * Save an image with cv image type of 'type' to file fname
     */
    void saveImageC( Pointer img, int width, int height, int type, char *fname, int fnameLen);
    /*
     * Start a video-save session to file fname
     */
    void videoSaveBeginC( int width, int height, int fourcc, double fps, Bool color,
                         Pointer fname, int fnameLen );
    /*
     * Save a frame of video. videoSaveBeginC() must be called first
     */
    void saveVideoC( Pointer img, int width, int height, int type );
    /*
     * Finalize saving a video. videoSaveBeginC() must be called first, followed
     * by saveVideoC() calls for any frames
     */
    void videoSaveEndC();
    /*
     * Display a histogram. histImage is a normalized-histogram array
     */
    void showNormalHistogramC( Pointer histImg, int width, int height );
}

#endif
