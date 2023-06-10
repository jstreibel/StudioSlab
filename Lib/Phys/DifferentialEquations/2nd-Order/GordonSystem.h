#ifndef HamiltonCPU_H
#define HamiltonCPU_H

#include "Phys/DifferentialEquations/DifferentialEquation.h"

#include "Phys/Numerics/Allocator.h"


#define FType(a) typename FieldState::a

namespace Phys {

    namespace Gordon {

        template<class FieldState>
        class GordonSystem : public Base::DifferentialEquation<FieldState> {
        protected:
            typedef Base::Function<FType(FunctionType::OutCategory),
                    FType(FunctionType::OutCategory)> TargetToTargetFunction;
        public:
            explicit GordonSystem(TargetToTargetFunction &potential)
                    : laplacian(
                    *(FType(FunctionArbitraryType) *) Numerics::Allocator::getInstance().newFunctionArbitrary()),
                      dV_out(*(FType(
                              FunctionArbitraryType) *) Numerics::Allocator::getInstance().newFunctionArbitrary()),
                      V(potential), dVDPhi(potential.diff(0)) {}

            ~GordonSystem() override {
                delete &laplacian;
                delete &dV_out;
                delete &V;
            }


            FieldState &
            dtF(const FieldState &fieldStateIn, FieldState &fieldStateOut, Real t, Real dt) override {
                const auto &iPhi = fieldStateIn.getPhi();
                const auto &iDPhi = fieldStateIn.getDPhiDt();
                auto &oPhi = fieldStateOut.getPhi();
                auto &oDPhi = fieldStateOut.getDPhiDt();

                // Eq 1
                { oPhi.SetArb(iDPhi) *= dt; }

                // Eq 2
                {
                    iPhi.Laplacian(laplacian);
                    iPhi.Apply(*dVDPhi, dV_out);

                    oDPhi.StoreSubtraction(laplacian, dV_out) *= dt;
                }

                return fieldStateOut;
            }

            static bool isDissipating() { return false; }

        protected:
            FType(FunctionArbitraryType) &laplacian, &dV_out;
            TargetToTargetFunction &V;
            TargetToTargetFunction::Ptr dVDPhi;
        };

    }

}


#endif // HamiltonCPU_H
