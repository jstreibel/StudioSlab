//
// Created by joao on 10/16/24.
//

#ifndef STUDIOSLAB_DYNAMICSNUMERICCONFIG_H
#define STUDIOSLAB_DYNAMICSNUMERICCONFIG_H

#include "Math/Numerics/ODE/SimConfig/NumericConfig.h"
#include "Core/Controller/CommandLine/CommonCLParameters.h"
#include "Utils/List.h"

namespace Slab::Models {

    using namespace Core;

    class DynamicsNumericConfig : public Math::NumericConfig {
    protected:
        IntegerParameter::Ptr N = IntegerParameter::New(1024, "N", "Discretization of space dimensions. "
                                                                   "Has to be POT for GPU");
        RealParameter::Ptr L = RealParameter::New(10., "L", "Space length");
        RealParameter::Ptr t = RealParameter::New(-1, "t", "Max simulation time. If left negative, defaults to L/2. "
                                                           "Note that this value can be overriden by simulation.");

    public:
        explicit DynamicsNumericConfig(bool do_register);

        auto getN() const -> UInt;
        auto getL() const -> Real;

        auto gett() const -> Real;
        auto sett(Real) -> void;

        auto notifyCLArgsSetupFinished() -> void override;
    };

} // Models

#endif //STUDIOSLAB_DYNAMICSNUMERICCONFIG_H
