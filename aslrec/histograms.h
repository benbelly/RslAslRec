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
