//
// Created by joao on 2/09/23.
//

#ifndef STUDIOSLAB_DISCRETEFOURIERTRANSFORM_H
#define STUDIOSLAB_DISCRETEFOURIERTRANSFORM_H

#include <memory>
#include <utility>

#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunction.h"

namespace Slab::Math::RtoR {

    typedef Math::FPointSet_ptr DFTModes;
    typedef CountType NumberOfModes;

    struct DFTResult {
        DFTModes re;
        DFTModes im;

        DFTModes getMagnitudes() const {
            IN R = re->GetPoints();
            IN I = im->GetPoints();

            assert(I.size() == R.size());

            fix N = R.size();

            auto amps = Slab::New<Math::FPointSet>();
            for(int i=0; i<N; ++i){
                fix Re_A = R[i].y;
                fix Im_A = I[i].y;
                fix ω = R[i].x;

                fix A = sqrt(Re_A*Re_A+Im_A*Im_A);

                amps->AddPoint({ω, A});
            }

            return amps;
        }

        CountType modeCount() const {
            assert(re->Count() == im->Count());

            fix n = re->Count();

            if(inverseIsReal) return (n-1)*2;

            return n;
        }

        explicit DFTResult(bool functionIsReal=false, DFTModes re=Slab::New<Math::FPointSet>(), DFTModes im=Slab::New<Math::FPointSet>())
        : re(std::move(re)), im(std::move(im)), inverseIsReal(functionIsReal)
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
        static auto
        Compute(const RtoR::Function &in, NumberOfModes N, DevFloat xMin, DevFloat xMax)
        -> DFTResult;

        static auto
        Compute(const RtoR::NumericFunction &in)
        -> DFTResult;

        static TPointer<RtoR::NumericFunction> Magnitudes(const DFTResult&);
    };


} // RtoR

#endif //STUDIOSLAB_DISCRETEFOURIERTRANSFORM_H
