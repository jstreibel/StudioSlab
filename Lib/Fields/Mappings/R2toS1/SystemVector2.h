#ifndef HamiltonCPU_H
#define HamiltonCPU_H

#include "Base/Math/System/System.h"
#include "R2toS1FieldState.h"

#include "Base/Math/Allocator.h"


#define FType(a) typename FieldState::a

namespace R2toS1 {

    class SystemGordon : public Base::System<R2toS1::FieldState> {
    private:
        typedef Base::Function<FType(FunctionType::TgtSpaceType),FType(FunctionType::TgtSpaceType)> TargetToTargetFunction;
    public:
    SystemGordon(TargetToTargetFunction &V)
            : temp1(*(FType(FunctionArbitraryType)*)ModelBuilder::getInstance().newFunctionArbitrary()),
              temp2(*(FType(FunctionArbitraryType)*)ModelBuilder::getInstance().newFunctionArbitrary()),
              V(V), dVDPhi(*V.diff(0).release()) {
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
            auto &laplacian = iPhi.Laplacian(temp1);
            auto &dV = iPhi.Apply(dVDPhi, temp2);

            oDPhi.StoreSubtraction(laplacian, dV) *= dt;
        }

        return fieldStateOut;
    }

    ~SystemGordon() override {
        delete &temp1;
        delete &temp2;
        delete &V;
        delete &dVDPhi;
    }

    static bool isDissipating() { return false; }

    private:
        FType(FunctionArbitraryType) &temp1, &temp2;
        TargetToTargetFunction &V;
        TargetToTargetFunction &dVDPhi;
    };

}


#endif // HamiltonCPU_H
