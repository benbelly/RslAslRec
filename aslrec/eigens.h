#ifndef _EIGENTHINGUMMIES_H__
#define _EIGENTHINGUMMIES_H__

#include "frame.h"
#include "histograms.h"


typedef std::vector<cv::PCA> PcaSet;

cv::PCA pcaForHist( Histogram h );
PcaSet pcaForHists( HistogramSet &hs );

cv::PCA pcaForTraining( HistogramSet hs );

#endif
