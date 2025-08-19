//
// Created by joao on 10/16/24.
//

#ifndef STUDIOSLAB_DYNAMICSNUMERICCONFIG_H
#define STUDIOSLAB_DYNAMICSNUMERICCONFIG_H

#include "Math/Numerics/ODE/SimConfig/NumericConfig.h"
#include "../Core/Controller/CommonParameters.h"
#include "Utils/List.h"

namespace Slab::Models {

    using namespace Core;

    class DynamicsNumericConfig : public Math::FNumericConfig {
    protected:
        TPointer<IntegerParameter> N = New<IntegerParameter>(1024,
            FParameterDescription{'N', "Discretization of space dimensions. Has to be POT for GPU"});
        TPointer<RealParameter> L = New<RealParameter>(10.,
            FParameterDescription{'L', "Space length"});
        TPointer<RealParameter> t = New<RealParameter>(-1,
            FParameterDescription{'t', "Max simulation time. If left negative, defaults to L/2. Note that this value can be overridden by simulation."});

    public:
        explicit DynamicsNumericConfig(bool do_register);

        auto getN() const -> UInt;
        auto GetL() const -> DevFloat;

        auto Get_t() const -> DevFloat;
        auto sett(DevFloat) -> void;

        auto NotifyCLArgsSetupFinished() -> void override;
    };

} // Models

#endif //STUDIOSLAB_DYNAMICSNUMERICCONFIG_H
