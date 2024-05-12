//
// Created by joao on 2/09/23.
//

#ifndef STUDIOSLAB_DISCRETEFOURIERTRANSFORM_H
#define STUDIOSLAB_DISCRETEFOURIERTRANSFORM_H

#include <memory>
#include <utility>

#include "Math/Function/Maps/RtoR/Model/RtoRDiscreteFunction.h"

namespace RtoR {

    typedef std::shared_ptr<Spaces::PointSet> DFTModes;
    typedef Count NumberOfModes;

    struct DFTResult {
        DFTModes re;
        DFTModes im;

        DFTModes getMagnitudes() const {
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
        }

        Count modeCount() const {
            assert(re->count() == im->count());

            fix n = re->count();

            if(inverseIsReal) return (n-1)*2;

            return n;
        }

        explicit DFTResult(bool functionIsReal=false, DFTModes re=std::make_shared<Spaces::PointSet>(), DFTModes im=std::make_shared<Spaces::PointSet>())
        : re(re), im(im), inverseIsReal(functionIsReal)
        {

        }

        [[nodiscard]] bool isInverseReal() const { return inverseIsReal; }

    private:
        bool inverseIsReal;

    };

    /**
     * Discrete Fourier transform, the one and only
     */
    class DFT {
    public:
        static DFTResult Compute(const RtoR::Function &in, NumberOfModes N, Real xMin, Real xMax);
        static DFTResult Compute(const RtoR::DiscreteFunction &in);
    };

} // RtoR

#endif //STUDIOSLAB_DISCRETEFOURIERTRANSFORM_H
