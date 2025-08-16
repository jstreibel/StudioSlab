//
// Created by joao on 26/03/25.
//

#ifndef SPI_NUMERIC_CONFIG_H
#define SPI_NUMERIC_CONFIG_H
#include <Math/Numerics/ODE/SimConfig/NumericConfig.h>

namespace Slab::Models::StochasticPathIntegrals {

    class SPINumericConfig : public Slab::Math::FNumericConfig {
        Core::RealParameter     L  = Core::RealParameter(1.0,     Core::FParameterDescription{'L', "length", "Space length"});
        Core::RealParameter     t  = Core::RealParameter(1.0,     Core::FParameterDescription{'t', "time", "Time interval"});
        Core::IntegerParameter  N  = Core::IntegerParameter(1000, Core::FParameterDescription{'N', "site_count", "Total sites in space direction. Time sites are computed proportionally from this."});
        Core::RealParameter     dT = Core::RealParameter(0.1,     Core::FParameterDescription{'d', "dT", "Stochastic time-step size"});
        Core::IntegerParameter  nT = Core::IntegerParameter(1000, Core::FParameterDescription{'n', "stochastic_time_steps", "Stochastic time-steps to be taken until expected end of transient"});

    public:
        SPINumericConfig();

        auto getL() const -> DevFloat;
        auto gett() const -> DevFloat;
        auto getN() const -> UInt;
        auto getdT() const -> DevFloat;
        auto getn() const -> UInt override;

        auto to_string() const -> Str override;
    };

}

#endif //SPI_NUMERIC_CONFIG_H
