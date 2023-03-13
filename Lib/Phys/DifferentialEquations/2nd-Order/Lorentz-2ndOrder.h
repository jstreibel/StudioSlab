#ifndef HamiltonCPU_H
#define HamiltonCPU_H

#include "Phys/Numerics/Equation/Equation.h"

#include "Phys/Numerics/Allocator.h"


#define FType(a) typename FieldState::a

namespace Base {

    template <class FieldState>
    class Lorentz_2ndOrder : public Base::Equation<FieldState> {
    private:
        typedef Base::Function<FType(FunctionType::OutCategory),FType(FunctionType::OutCategory)> TargetToTargetFunction;
    public:
    explicit Lorentz_2ndOrder(TargetToTargetFunction &potential)
            : laplacian(*(FType(FunctionArbitraryType)*)Allocator::getInstance().newFunctionArbitrary()),
              dV(*(FType(FunctionArbitraryType)*)Allocator::getInstance().newFunctionArbitrary()),
              V(potential), dVDPhi(*potential.diff(0).release()) {   }

    ~Lorentz_2ndOrder() override {
        delete &laplacian;
        delete &dV;
        delete &V;
        delete &dVDPhi;
    }

    FieldState &dtF(const FieldState &fieldStateIn, FieldState &fieldStateOut, Real t, Real dt) override {
        const auto &iPhi = fieldStateIn.getPhi();
        const auto &iDPhi = fieldStateIn.getDPhiDt();
        auto &oPhi = fieldStateOut.getPhi();
        auto &oDPhi = fieldStateOut.getDPhiDt();

        // Eq 1
        {   oPhi.SetArb(iDPhi) *= dt;   }

        // Eq 2
        {
            iPhi.Laplacian(laplacian);
            iPhi.Apply(dVDPhi, dV);

            oDPhi.StoreSubtraction(laplacian, dV) *= dt;
        }

        return fieldStateOut;
    }


    static bool isDissipating() { return false; }

    protected:
        FType(FunctionArbitraryType) &laplacian, &dV;
        TargetToTargetFunction &V;
        TargetToTargetFunction &dVDPhi;
    };

}


#endif // HamiltonCPU_H
