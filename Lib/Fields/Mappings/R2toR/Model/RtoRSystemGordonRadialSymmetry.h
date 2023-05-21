//
// Created by joao on 19/11/2019.
//

#ifndef V_SHAPE_RTORSYSTEMGORDONRADIALSYMMETRY_H
#define V_SHAPE_RTORSYSTEMGORDONRADIALSYMMETRY_H

#include <Base/Math/System/System.h>
#include <Base/Math/Core/FunctionsBase.h>
#include "RtoRSystemGordonRadialSymmetry.h"
#include "Lib/Fields/Maps/RtoR/Model/RtoRFieldState.h"
#include "Base/Math/Allocator.h"

namespace RtoR {
    class SystemGordonRadialSymmetry : public Base::System<RtoR::FieldState> {
    public:
        SystemGordonRadialSymmetry(RtoR::Function &V)
                : temp1(*(RtoR::FunctionArbitrary*)Allocator::getInstance().newFunctionArbitrary()),
                  temp2(*(RtoR::FunctionArbitrary*)Allocator::getInstance().newFunctionArbitrary()),
                  V(V), dVDPhi(*V.diff(0).release()) {}
        ~SystemGordonRadialSymmetry() override {
            delete &temp1;
            delete &temp2;
            delete &V;
            delete &dVDPhi;
        }

        FieldState &dtF(const FieldState &fieldStateIn, FieldState &fieldStateOut, Real t, Real dt) override {
            const auto &iPhi = fieldStateIn.getPhi();
            const auto &iDPhi = fieldStateIn.getDPhiDt();
            auto &oPhi = fieldStateOut.getPhi();
            auto &oDPhi = fieldStateOut.getDPhiDt();

            // Eq 1
            {
                oPhi.SetArb(iDPhi) *= dt;
            }

            // Eq 2
            {
                const auto& vPhi = iPhi.getSpace().getX();
                //const auto &laplacian = iPhi.Laplacian(temp1).getSpace().getX();
                const auto &phidV = iPhi.Apply(dVDPhi, temp2).getSpace().getX();

                auto &oVec = oDPhi.getSpace().getX();
                const Real invhsqr = 1./iPhi.getSpace().geth();

                OMP_PARALLEL_FOR(i, oVec.size()){
                    const Real lapl = invhsqr * ((vPhi[i-1] + vPhi[i+1]) - 2.0*vPhi[i]);

                    oVec[i] = dt*(lapl - phidV[i]);
                }
            }

            return fieldStateOut;
        }


    private:
        RtoR::FunctionArbitrary &temp1, &temp2;
        RtoR::Function &V;
        RtoR::Function &dVDPhi;
    };
}


#endif //V_SHAPE_RTORSYSTEMGORDONRADIALSYMMETRY_H
