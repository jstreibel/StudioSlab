//
// Created by joao on 10/16/24.
//

#ifndef STUDIOSLAB_KG_NUMERICCONFIG_H
#define STUDIOSLAB_KG_NUMERICCONFIG_H

#include "Core/Controller/CommandLine/CommandLineCommonParameters.h"
#include "Models/DynamicsNumericConfig.h"

namespace Slab::Models {

    using namespace Core;

    class KGNumericConfig : public DynamicsNumericConfig {

        RealParameter::Ptr xCenter = RealParameter::New(.0, "c,xCenter", "Center of simulation space");

        IntegerParameter::Ptr dimMode = IntegerParameter::New(0, "m,dimensions_mode",
                                                              "Method to compute space measures:"
                                                              "\n\t0: h=L/N (option --h is ignored)"
                                                              "\n\t1: L=h*N (option --L is ignored)"
                                                              "\n\t2: N=L/h (option --N is ignored)"
                                                              "\nNote that mode 2 is not too safe if N is "
                                                              "not Natural number.");

        RealParameter::Ptr h = RealParameter::New(0.009765625, "h", "Cell 1-measure. Defaults to L/N. This value is "
                                                                    "ignored unless '--mode' is properly set.");

        RealParameter::Ptr rdt = RealParameter::New(.1, "r_dt", "Timestep 'r' parameter such that dt=r*h.");
        UInt n{};

    public:
        explicit KGNumericConfig(bool doRegister = true);

        KGNumericConfig(const KGNumericConfig &) = delete;

        auto to_string() const -> Str override;

        auto getn() const -> UInt override;

        auto getdt() const -> DevFloat;

        auto getr() const -> DevFloat;

        auto getxMin() const -> DevFloat;

        auto getxMax() const -> DevFloat;

        auto geth() const -> DevFloat;

        auto NotifyCLArgsSetupFinished() -> void override;


    };

} // Slab::Models::KleinGordon

#endif //STUDIOSLAB_KG_NUMERICCONFIG_H
