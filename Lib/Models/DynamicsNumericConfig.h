//
// Created by joao on 10/16/24.
//

#ifndef STUDIOSLAB_DYNAMICSNUMERICCONFIG_H
#define STUDIOSLAB_DYNAMICSNUMERICCONFIG_H

#include "Math/Numerics/ODE/SimConfig/NumericConfig.h"
#include "../Core/Controller/Parameter/BuiltinParameters.h"
#include "Core/Controller/Parameter/ParameterBuilder.h"
#include "Utils/List.h"

namespace Slab::Models {

    using namespace Core;

    class DynamicsNumericConfig : public Math::FNumericConfig {
    protected:
        TPointer<IntegerParameter> N = MakeIntParam(128,
            FParameterDescription{'N', "Number of Molecules to simulate"},
            FIntegerParameterOptions{128, 1<<18, false, true});
        TPointer<RealParameter> L = MakeRealParam(100.,
            FParameterDescription{'L', "Space length. Simulation will be an LxL box"},
            FRealParameterOptions{1e-2, 1.e5});
        TPointer<RealParameter> t = New<RealParameter>(-1,
            FParameterDescription{'t', "Max simulation time. If left negative, it is unbounded"});

    public:
        explicit DynamicsNumericConfig(bool do_register);

        auto getN() const -> UInt;
        auto GetL() const -> DevFloat;

        auto Get_t() const -> DevFloat;
        auto sett(DevFloat) -> void;

        auto NotifyInterfaceSetupIsFinished() -> void override;
    };

} // Models

#endif //STUDIOSLAB_DYNAMICSNUMERICCONFIG_H
