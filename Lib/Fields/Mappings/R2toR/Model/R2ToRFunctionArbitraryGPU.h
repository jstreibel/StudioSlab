//
// Created by joao on 27/09/2019.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H
#define V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H

#include "R2toRFunctionArbitrary.h"
#include "Studios/CoreMath/Util/device-config.h"

namespace R2toR {

    class FunctionArbitraryGPU : public R2toR::FunctionArbitrary {
    public:
        FunctionArbitraryGPU(PosInt N, Real sMin, Real h);

        FunctionArbitrary &Laplacian(FunctionArbitrary &outFunc) const override;

        [[nodiscard]] Function<Real2D, Real> *Clone() const override;
        [[nodiscard]] Base::FunctionArbitrary<Real2D, Real> *CloneWithSize(PosInt N) const override;

        FunctionArbitraryBase &Set(const MyBase &func) override;
        FunctionArbitraryBase &SetArb(const FunctionArbitraryBase &func) override;
        FunctionArbitraryBase &Apply(const Function<Real, Real> &func, FunctionArbitraryBase &out) const override;

        FunctionArbitraryBase &Add(const FunctionArbitraryBase & toi) override;
        FunctionArbitraryBase &StoreAddition(const FunctionArbitraryBase &toi1,
                                             const FunctionArbitraryBase & toi2) override;
        FunctionArbitraryBase &StoreSubtraction(const FunctionArbitraryBase &aoi1,
                                                const FunctionArbitraryBase& aoi2) override;
        FunctionArbitraryBase &Multiply(floatt a) override;

        Real At(PosInt n, PosInt m) const override;
        Real &At(PosInt n, PosInt m) override;

    private:
        bool hostIsUpdated = false;
        VecFloat &XHost;
        DeviceVector &XDev;

    };

}

#endif //V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H
