//
// Created by joao on 2/09/23.
//

#ifndef STUDIOSLAB_FOURIERTRANSFORM_H
#define STUDIOSLAB_FOURIERTRANSFORM_H

#include <memory>
#include <utility>

#include "Mappings/RtoR/Model/RtoRDiscreteFunction.h"

namespace RtoR {

    typedef std::shared_ptr<Spaces::PointSet> DFTModes;
    typedef Count NumberOfModes;

    struct DFTResult {
        explicit DFTResult(DFTModes re=std::make_shared<Spaces::PointSet>(), DFTModes im=std::make_shared<Spaces::PointSet>())
        : re(std::move(re)), im(std::move(im)) {}

        DFTModes re;
        DFTModes im;
    };

    class FourierTransform {
    public:
        static DFTResult Compute(const RtoR::Function &in, NumberOfModes N, Real xMin, Real xMax);
        static DFTResult Compute(const RtoR::DiscreteFunction &in);
    };

} // R2toR

#endif //STUDIOSLAB_FOURIERTRANSFORM_H
