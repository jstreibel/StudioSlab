//
// Created by joao on 26/03/25.
//

#ifndef SPI_NUMERIC_CONFIG_H
#define SPI_NUMERIC_CONFIG_H
#include <Math/Numerics/ODE/SimConfig/NumericConfig.h>

namespace Slab::Models::StochasticPathIntegrals {

    class SPINumericConfig : public Slab::Math::NumericConfig {
        Core::RealParameter L = Core::RealParameter(1.0, "L,length", "Space length");
        Core::RealParameter t = Core::RealParameter(1.0, "t,time", "Time interval");
        Core::IntegerParameter N = Core::IntegerParameter(1000, "N,site_count", "Total sites in space "
                                                                         "direction. Time sites are computed "
                                                                         "proportionally from this.");
        Core::RealParameter dT = Core::RealParameter(0.1, "d,dT", "Stochastic time-step size");
        Core::IntegerParameter nT = Core::IntegerParameter(1000, "n,stochastic_time_steps",
            "Stochastic time-steps to be taken until expected end of transient");

    public:
        SPINumericConfig();

        auto getL() const -> Real;
        auto gett() const -> Real;
        auto getN() const -> UInt;
        auto getdT() const -> Real;
        auto getn() const -> UInt override;

        auto to_string() const -> Str override;
    };

}

#endif //SPI_NUMERIC_CONFIG_H
