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


#include "FeatureFrame.h"
#include "edgedetection.h"
#include "consts.h"
#include <iostream>

FeatureFrame::FeatureFrame( cv::Point tl, cv::Point br, Contour d,
                            boost::shared_ptr<Contour> w ) :
    faceTopLeft( tl ), faceBottomRight( br ),
    dom( d ), weak( w ),
    domHist( generateHandHistogram( dom, weak.get() ) )
{
}

double FeatureFrame::distance( const cv::Mat &testHands,
                               const cv::PCA &pca, const cv::Mat &covar ) {
    return cv::Mahalanobis( testHands, GetDomProjection( pca ), covar );
}

const cv::Mat &FeatureFrame::GetDomProjection( const cv::PCA &pca ) {
    if( !projection.get() ) {
        projection.reset( new cv::Mat( pca.project( flattenHistogram( domHist ) ) ) );
    }
    return *(projection.get());
}

