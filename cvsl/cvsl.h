/**
    Copyright 2011 Ben Holm

    This file is part of RslAslRec.

    RslAslRec is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RslAslRec is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RslAslRec.  If not, see <http://www.gnu.org/licenses/>.
**/


#ifndef CVSL_H__
#define CVSL_H__

#include "export.h" // mlton types
#include "ml-types.h" // mlton types, for real

/*****************************************************************************
 *
 * These functions are specific to the problem domain an must be implemented
 * by the CVSL user
 *
 ****************************************************************************/
extern "C" {
    /*
     * Get the number of frames of a particular type
     * InitAslAlg() must be called first
     */
    int numFramesC( int type );
    /*
     * Get the ids associated with frames of a particular type
     * InitAslAlg() must be called first
     */
    void getFrameIdsC( int type, Pointer ids );
    /*
     * Return the width, height, cv image type, and size of a type of images
     * InitAslAlg() must be called first
     */
    void getFrameInfoC( int type, Pointer width, Pointer height,
                                  Pointer dtype, Pointer size );
    /*
     * Return the image with id and type
     * InitAslAlg() must be called first
     */
    void getFrameC( int id, int type, Pointer img );
}

/*****************************************************************************
 *
 * These functions are provided using OpenCV and PLPlot
 *
 ****************************************************************************/
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
     * Display a histogram. histImage is a normalized-histogram array
     */
    void showNormalHistogramC( Pointer histImg, int width, int height );
}

#endif
