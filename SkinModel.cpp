
#include "SkinModel.h"
#include "consts.h"
#include "edgedetection.h"
#include <stdlib.h>
#include <memory>
#include <utility>

std::pair<int, double *> loadSkinModel() {
    FILE *in = fopen( "skinmodel.model", "rb" );
    double step = 0;
    fread( &step, sizeof(double), 1, in );

    int l = 256 / step;
    int arrSize = l * l * l;
    double* model = new double[arrSize];
    fread( model, sizeof(double), arrSize, in);

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
