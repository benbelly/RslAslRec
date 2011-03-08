

#include "writer.h"
#include "frame.h"
#include "histograms.h"
#include <string>
#include <sstream>
#include <vector>

void writeMats( const std::string filename, const std::vector<Histogram> &frames ) {
    for( unsigned int i = 0; i < frames.size(); ++i ) {
        std::ostringstream os;
        os << filename << i << ".yml";
        std::string fname = os.str();
        cv::FileStorage fs( fname, cv::FileStorage::WRITE );
        fs << "mtx" << frames[i];
    }
}
