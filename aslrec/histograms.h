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

#ifndef HISTOGRAMS_H__
#define HISTOGRAMS_H__

#include <map>
#include <string>

#include "frame.h"

typedef cv::Mat_<double> Histogram;
typedef std::vector<Histogram> HistogramSet;

typedef cv::Mat Projection;
typedef std::vector<cv::Mat> ProjectionSet; // For histograms projected to PCA space

Histogram generateHandHistogram( const Contour hand1, const Contour *hand2 );
HistogramSet generateHandHistograms( const ContourSet cs );
Histogram flattenHistogram( const Histogram &h );

cv::Mat h2i( Frame f, HistogramSet &hs );

#endif
