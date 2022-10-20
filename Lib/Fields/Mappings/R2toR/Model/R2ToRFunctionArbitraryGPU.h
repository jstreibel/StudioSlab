//
// Created by joao on 27/09/2019.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H
#define V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H

#include "R2toRFunctionArbitrary.h"


namespace R2toR {

    class FunctionArbitraryGPU : public R2toR::FunctionArbitrary {
    public:
        FunctionArbitraryGPU(PosInt N, Real sMin, Real h);

        FunctionArbitrary &Laplacian(FunctionArbitrary &outFunc) const override;

        [[nodiscard]] Function<Real2D, Real> *Clone() const override;
        [[nodiscard]] Base::ArbitraryFunction<Real2D, Real> *CloneWithSize(PosInt N) const override;

        ArbitraryFunction &Set(const MyBase &func) override;
        ArbitraryFunction &SetArb(const ArbitraryFunction &func) override;
        ArbitraryFunction &Apply(const Function<Real, Real> &func, ArbitraryFunction &out) const override;

        ArbitraryFunction &Add(const ArbitraryFunction & toi) override;
        ArbitraryFunction &StoreAddition(const ArbitraryFunction &toi1,
                                             const ArbitraryFunction & toi2) override;
        ArbitraryFunction &StoreSubtraction(const ArbitraryFunction &aoi1,
                                                const ArbitraryFunction& aoi2) override;
        ArbitraryFunction &Multiply(floatt a) override;

        Real At(PosInt n, PosInt m) const override;
        Real &At(PosInt n, PosInt m) override;

        void updateHost();
        void updateDevice();
        bool isSyncd() const;

    private:
        bool hostIsSyncd = false;
        // TODO: Nao usar mais esses, e sim o do this->space.
        //VecFloat &XHost;
        //DeviceVector &XDev;

    };

}

#endif //V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H
