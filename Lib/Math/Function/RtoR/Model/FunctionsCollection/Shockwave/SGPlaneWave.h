//
// Created by joao on 8/3/25.
//

#ifndef SG_PLANEWAVE_H
#define SG_PLANEWAVE_H
#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    class FSignumGordonPlaneWave final : public  RtoR::Function {
        DevFloat Q;
        DevFloat k;

    public:

        FSignumGordonPlaneWave(const DevFloat Q, const DevFloat k);

        FSignumGordonPlaneWave(const FSignumGordonPlaneWave& other);

        FSignumGordonPlaneWave(FSignumGordonPlaneWave&& other) noexcept;

        FSignumGordonPlaneWave& operator=(const FSignumGordonPlaneWave& other);

        FSignumGordonPlaneWave& operator=(FSignumGordonPlaneWave&& other) noexcept;

        DevFloat operator()(DevFloat x) const override;
    };

    class FSignumGordonPlaneWave_TimeDerivative final : public RtoR::Function {
        DevFloat Q;
        DevFloat k;

    public:
        FSignumGordonPlaneWave_TimeDerivative(const DevFloat Q, const DevFloat k);

        FSignumGordonPlaneWave_TimeDerivative(const FSignumGordonPlaneWave_TimeDerivative& other);

        FSignumGordonPlaneWave_TimeDerivative(FSignumGordonPlaneWave_TimeDerivative&& other) noexcept;

        FSignumGordonPlaneWave_TimeDerivative& operator=(const FSignumGordonPlaneWave_TimeDerivative& other);

        FSignumGordonPlaneWave_TimeDerivative& operator=( FSignumGordonPlaneWave_TimeDerivative&& other) noexcept;

        DevFloat operator()(DevFloat x) const override;
    };

} // Slab::Math::RtoR

#endif //SG_PLANEWAVE_H
