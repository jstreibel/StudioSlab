//
// Created by joao on 2/09/23.
//

#ifndef STUDIOSLAB_FOURIERTRANSFORM_H
#define STUDIOSLAB_FOURIERTRANSFORM_H

#include <memory>
#include <utility>

#include "Maps/RtoR/Model/RtoRDiscreteFunction.h"

namespace RtoR {

    typedef std::shared_ptr<Spaces::PointSet> DFTModes;
    typedef Count NumberOfModes;

    struct DFTResult {
        DFTModes re;
        DFTModes im;

        DFTModes getAbs() const {
            IN R = re->getPoints();
            IN I = im->getPoints();

            assert(I.size() == R.size());

            fix N = R.size();

            auto amps = std::make_shared<Spaces::PointSet>();
            for(int i=0; i<N; ++i){
                fix Re_A = R[i].y;
                fix Im_A = I[i].y;
                fix ω = R[i].x;

                fix A = sqrt(Re_A*Re_A+Im_A*Im_A);

                amps->addPoint({ω, A});
            }

            return amps;
        };

        explicit DFTResult(DFTModes re=std::make_shared<Spaces::PointSet>(), DFTModes im=std::make_shared<Spaces::PointSet>())
        : re(std::move(re)), im(std::move(im)) {}

    };

    class FourierTransform {
    public:
        static DFTResult Compute(const RtoR::Function &in, NumberOfModes N, Real xMin, Real xMax);
        static DFTResult Compute(const RtoR::DiscreteFunction &in);
    };

} // R2toR

#endif //STUDIOSLAB_FOURIERTRANSFORM_H
