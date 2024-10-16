//
// Created by joao on 10/16/24.
//

#ifndef STUDIOSLAB_KGNUMERICCONFIG_H
#define STUDIOSLAB_KGNUMERICCONFIG_H

#include "Math/Numerics/SimConfig/NumericConfig.h"
#include "Core/Controller/CommandLine/CommonCLParameters.h"

namespace Slab::Models {

    using namespace Core;

    class KGNumericConfig : public Math::NumericConfig {
        IntegerParameter::Ptr N = IntegerParameter::New(1024, "N", "Discretization of space dimensions. "
                                                                   "Has to be POT for GPU");
        RealParameter::Ptr L = RealParameter::New(10., "L", "Space length");
        RealParameter::Ptr xCenter = RealParameter::New(.0, "c,xCenter", "Center of simulation space");
        RealParameter::Ptr t = RealParameter::New(-1, "t", "Max simulation time. If left negative, defaults to L/2. "
                                                           "Note that this value can be overriden by simulation.");

        IntegerParameter::Ptr dimMode = IntegerParameter::New(0, "m,dimensions_mode",
                                                              "Method to compute space measures:"
                                                              "\n\t0: h=L/N (option --h is ignored)"
                                                              "\n\t1: L=h*N (option --L is ignored)"
                                                              "\n\t2: N=L/h (option --N is ignored)"
                                                              "\nNote that mode 2 is not too safe if N is "
                                                              "not Natural number.");

        RealParameter::Ptr h = RealParameter::New(0.009765625, "h", "Cell 1-measure. Defaults to L/N. This value is "
                                                                    "ignored unless '--mode' is properly set.");

        RealParameter::Ptr rdt = RealParameter::New(-1, "r_dt", "Timestep 'r' parameter such that dt=r*h. If negative value, then it"
                                                                "is set to r=1/10.");

        UInt n{};

    public:
        explicit KGNumericConfig(bool doRegister = true);

        KGNumericConfig(const KGNumericConfig &) = delete;

        auto getn() const -> UInt override;

        auto to_string() const -> Str override;


        auto getN() const -> UInt;

        auto getL() const -> Real;

        auto getxMin() const -> Real;

        auto getxMax() const -> Real;

        auto gett() const -> Real;

        auto getr() const -> Real;

        auto geth() const -> Real;

        auto getdt() const -> Real;

        void sett(Real tMax) const;

        auto notifyCLArgsSetupFinished() -> void override;


    };

} // Slab::Models::KleinGordon

#endif //STUDIOSLAB_KGNUMERICCONFIG_H
