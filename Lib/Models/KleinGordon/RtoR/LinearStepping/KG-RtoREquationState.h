//
// Created by joao on 05/09/2019.
//

#ifndef V_SHAPE_EQSTATERTOR_H
#define V_SHAPE_EQSTATERTOR_H

#include <utility>

#include "Models/KleinGordon/KG-State.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunction.h"

namespace Slab::Models::KGRtoR {



    class EquationState : public Models::KGState<DevFloat> {
        typedef Models::KGState<DevFloat>::Field_ptr Field_ptr;
    public:
        EquationState(Field_ptr phi, Field_ptr dPhiDt)
        : Models::KGState<DevFloat>(std::move(phi), std::move(dPhiDt)) {}

        void outputPhi(OStream &out, Str separator) const override {
            const RealArray &vPhi = getPhi().getSpace().getHostData();
            const UInt N = vPhi.size();

            for(UInt n=0; n<N; n++)
                out << vPhi[n] << separator;
        }

        void outputdPhiDt(OStream &out, Str separator) const override {
            const RealArray &vDPhiDt = getDPhiDt().getSpace().getHostData();
            const UInt N = vDPhiDt.size();

            for(UInt n=0; n<N; n++)
                out << vDPhiDt[n] << separator;
        }

        [[nodiscard]] EqStateOutputInterface *Copy(UInt N) const override {
            NOT_IMPLEMENTED_CLASS_METHOD
            // new EquationState(dynamic_cast<RtoR::NumericFunction*>(phi->CloneWithSize(N)),
            //                   dynamic_cast<RtoR::NumericFunction*>(dPhiDt->CloneWithSize(N)));

            return nullptr;
        }

        [[nodiscard]] auto replicate() const -> TPointer<Base::EquationState> override;

        [[nodiscard]] auto category() const -> Str override;
    };

    DefinePointers(EquationState)
}

#endif //V_SHAPE_EQSTATERTOR_H
