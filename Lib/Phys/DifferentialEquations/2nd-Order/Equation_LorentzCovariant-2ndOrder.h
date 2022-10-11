#ifndef HamiltonCPU_H
#define HamiltonCPU_H

#include "Phys/Numerics/Equation/Equation.h"

#include "Phys/Numerics/Allocator.h"


#define FType(a) typename FieldState::a

namespace Base {

    template <class FieldState>
    class Equation_LorentzCovariant_2ndOrder : public Base::Equation<FieldState> {
    private:
        typedef Base::Function<FType(FunctionType::OutCategory),FType(FunctionType::OutCategory)> TargetToTargetFunction;
    public:
    explicit Equation_LorentzCovariant_2ndOrder(TargetToTargetFunction &potential)
            : temp1(*(FType(FunctionArbitraryType)*)Allocator::getInstance().newFunctionArbitrary()),
              temp2(*(FType(FunctionArbitraryType)*)Allocator::getInstance().newFunctionArbitrary()),
              V(potential), dVDPhi(*potential.diff(0).release()) {   }

    ~Equation_LorentzCovariant_2ndOrder() override {
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
        {   oPhi.SetArb(iDPhi) *= dt;   }

        // Eq 2
        {
            auto &laplacian = iPhi.Laplacian(temp1);
            auto &dV = iPhi.Apply(dVDPhi, temp2);

            oDPhi.StoreSubtraction(laplacian, dV) *= dt;
        }

        return fieldStateOut;
    }


    static bool isDissipating() { return false; }

    private:
        FType(FunctionArbitraryType) &temp1, &temp2;
        TargetToTargetFunction &V;
        TargetToTargetFunction &dVDPhi;
    };

}


#endif // HamiltonCPU_H
