//
// Created by joao on 05/09/2019.
//

#ifndef V_SHAPE_FIELDSTATERTOR_H
#define V_SHAPE_FIELDSTATERTOR_H

#include "Model/FieldState.h"
#include "RtoRFunctionArbitrary.h"

namespace RtoR {

    class FieldState : public Base::FieldState<FunctionArbitrary> {
        // TODO rename FieldState to Field, since phi and dphidt are enough (analytically) to describe the
        //  entirety of the field in all of space and time.
    public:
        FieldState(FunctionArbitrary *phi, FunctionArbitrary *dPhiDt) : Base::FieldState<FunctionArbitrary>(phi, dPhiDt) {}

    public:
        void outputPhi(std::ostream &out, String separator) const override {
            const VecFloat &vPhi = getPhi().getSpace().getX();
            const PosInt N = vPhi.size();

            for(PosInt n=0; n<N; n++)
                out << vPhi[n] << separator;
        }

        void outputdPhiDt(std::ostream &out, String separator) const override {
            const VecFloat &vDPhiDt = getDPhiDt().getSpace().getX();
            const PosInt N = vDPhiDt.size();

            for(PosInt n=0; n<N; n++)
                out << vDPhiDt[n] << separator;
        }

        FStateOutputInterface *Copy(PosInt N) const override {
            return new FieldState(dynamic_cast<FunctionArbitrary*>(phi->CloneWithSize(N)),
                                  dynamic_cast<FunctionArbitrary*>(dPhiDt->CloneWithSize(N)));
        }
    };
}

#endif //V_SHAPE_FIELDSTATE_H
