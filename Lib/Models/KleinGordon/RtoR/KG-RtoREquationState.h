//
// Created by joao on 05/09/2019.
//

#ifndef V_SHAPE_EQSTATERTOR_H
#define V_SHAPE_EQSTATERTOR_H

#include "Models/KleinGordon/KGState.h"
#include "Math/Function/RtoR/Model/RtoRDiscreteFunction.h"

namespace Slab::Models::KGRtoR {

    using DiscreteFunky = RtoR::DiscreteFunction;
    using DiscreteFunky_ptr = RtoR::DiscreteFunction_ptr;

    class EquationState : public Models::KGState<DiscreteFunky> {
        // TODO rename FieldState to Field, since phi and dphidt are enough (analytically) to describe the
        //  entirety of the field in all of space and time.
    public:
        EquationState(DiscreteFunky_ptr phi, DiscreteFunky_ptr dPhiDt)
        : Models::KGState<DiscreteFunky>(phi, dPhiDt) {}

    public:
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

        EqStateOutputInterface *Copy(UInt N) const override {

            // new EquationState(dynamic_cast<RtoR::DiscreteFunction*>(phi->CloneWithSize(N)),
            //                   dynamic_cast<RtoR::DiscreteFunction*>(dPhiDt->CloneWithSize(N)));

            return nullptr;
        }

        auto clone() const -> Pointer<Base::EquationState>;
    };

    DefinePointer(EquationState)
}

#endif //V_SHAPE_EQSTATERTOR_H
