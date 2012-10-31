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


#include "SkinModel.h"
#include "consts.h"
#include "edgedetection.h"
#include <stdio.h>
#include <memory>
#include <utility>

std::pair<int, double *> loadSkinModel() {
    FILE *in = fopen( "skinmodel.model", "rb" );
    double step = 0;
    size_t  sz = fread( &step, sizeof(double), 1, in );
    if( !sz ) throw std::string( "Nothing read" );

    int l = 256 / step;
    int arrSize = l * l * l;
    double* model = new double[arrSize];
    sz = fread( model, sizeof(double), arrSize, in);
    if( !sz ) throw std::string( "Another nothing read" );

    return std::make_pair( step, model );
}

SkinModel::SkinModel() {
}

SkinModel::~SkinModel() {
}

Frame SkinModel::GetSkinMask( Frame &frame ) {
    Frame skinMask = GenerateRawMask( frame );
    return removeComponentsFromFrame( skinMask );
}

Frame SkinModel::GenerateRawMask( Frame &frame ) {
    std::pair<int, double *> stepAndModel = GetModel();
    int step = stepAndModel.first;
    int l = 256 / step;
    double *model = stepAndModel.second;
    cv::Mat skinMat = cv::Mat::zeros( frame.mat.size(), image_types::gray );
    Frame skin( frame.Id(), skinMat );
    for( int y = 0; y < frame.mat.rows; ++y ) {
        for( int x = 0; x < frame.mat.rows; ++x ) {
            cv::Vec3b chans = frame.mat.at<cv::Vec3b>( y, x );
            int b = chans[0] / step,
                g = chans[1] / step,
                r = chans[2] / step;
            double modelVal = model[b * l * l + g * l + r];
            skin.mat.at<unsigned char>( y, x ) = modelVal > skinColorThresh ? 255 : 0;
        }
    }
    return skin;
}

std::pair<int, double *> SkinModel::GetModel() const {
    // This will not be freed - does it need to be?
    static std::pair<int, double *> stepAndModel( 0, static_cast<double *>( 0 ) );
    if( !stepAndModel.second )
        stepAndModel = loadSkinModel();
    return stepAndModel;
}
