#ifndef _EIGENTHINGUMMIES_H__
#define _EIGENTHINGUMMIES_H__

#include "frame.h"

typedef std::vector<cv::Mat> SampleSet;

cv::PCA principalComponentAnalysis( SampleSet ss );

#endif
