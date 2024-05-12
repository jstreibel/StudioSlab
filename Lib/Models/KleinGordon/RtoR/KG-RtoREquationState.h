//
// Created by joao on 05/09/2019.
//

#ifndef V_SHAPE_FIELDSTATERTOR_H
#define V_SHAPE_FIELDSTATERTOR_H

#include "Models/KleinGordon/KGState.h"
#include "Math/Function/Maps/RtoR/Model/RtoRDiscreteFunction.h"

namespace RtoR {

    class EquationState : public Fields::KleinGordon::KGState<RtoR::DiscreteFunction> {
        // TODO rename FieldState to Field, since phi and dphidt are enough (analytically) to describe the
        //  entirety of the field in all of space and time.
    public:
        EquationState(DiscreteFunction *phi, DiscreteFunction *dPhiDt)
        : Fields::KleinGordon::KGState<DiscreteFunction>(phi, dPhiDt) {}

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
            return new EquationState(dynamic_cast<DiscreteFunction*>(phi->CloneWithSize(N)),
                                     dynamic_cast<DiscreteFunction*>(dPhiDt->CloneWithSize(N)));
        }

        auto clone() const -> EquationState *;
    };
}

#endif //V_SHAPE_FIELDSTATE_H
