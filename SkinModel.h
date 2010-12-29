
#ifndef SKINMODEL_H__
#define SKINMODEL_H__

#include "frame.h"
#include<memory>
#include<utility>

class SkinModel {
    public:
        SkinModel();
        virtual ~SkinModel();

        Frame GetSkinMask( Frame &frame );

    private:
        std::pair<int, double *> GetModel() const;
};

#endif
