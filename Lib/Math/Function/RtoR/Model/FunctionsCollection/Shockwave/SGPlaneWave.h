//
// Created by joao on 8/3/25.
//

#ifndef SG_PLANEWAVE_H
#define SG_PLANEWAVE_H
#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    class FSignumGordonPlaneWave final : public  RtoR::Function {
        Real Q;
        Real k;

    public:

        FSignumGordonPlaneWave(const Real Q, const Real k);

        FSignumGordonPlaneWave(const FSignumGordonPlaneWave& other);

        FSignumGordonPlaneWave(FSignumGordonPlaneWave&& other) noexcept;

        FSignumGordonPlaneWave& operator=(const FSignumGordonPlaneWave& other);

        FSignumGordonPlaneWave& operator=(FSignumGordonPlaneWave&& other) noexcept;

        Real operator()(Real x) const override;
    };

    class FSignumGordonPlaneWave_TimeDerivative final : public RtoR::Function {
        Real Q;
        Real k;

    public:
        FSignumGordonPlaneWave_TimeDerivative(const Real Q, const Real k);

        FSignumGordonPlaneWave_TimeDerivative(const FSignumGordonPlaneWave_TimeDerivative& other);

        FSignumGordonPlaneWave_TimeDerivative(FSignumGordonPlaneWave_TimeDerivative&& other) noexcept;

        FSignumGordonPlaneWave_TimeDerivative& operator=(const FSignumGordonPlaneWave_TimeDerivative& other);

        FSignumGordonPlaneWave_TimeDerivative& operator=( FSignumGordonPlaneWave_TimeDerivative&& other) noexcept;

        Real operator()(Real x) const override;
    };

} // Slab::Math::RtoR

#endif //SG_PLANEWAVE_H
