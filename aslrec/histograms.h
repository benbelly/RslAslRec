#ifndef HISTOGRAMS_H__
#define HISTOGRAMS_H__

#include <map>
#include <string>

#include "frame.h"

typedef cv::Mat_<double> Histogram;
typedef std::vector<Histogram> HistogramSet;

Histogram generateHandHistogram( cv::Size size, Contour c );
HistogramSet generateHandHistograms( cv::Size size, ContourSet cs );

cv::Mat h2i( Frame f, HistogramSet &hs );

#endif
