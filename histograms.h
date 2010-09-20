#ifndef HISTOGRAMS_H__
#define HISTOGRAMS_H__

#include <map>

#include "frame.h"

typedef std::vector<cv::Mat> HistogramSet;

HistogramSet generateHandHistograms( Frame f, ContourSet cs );
cv::Mat generateHandHistogram( Frame f, Contour c );

cv::Mat h2i( Frame f, HistogramSet &hs );
#endif
