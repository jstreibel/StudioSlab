//
// Created by joao on 10/16/24.
//

#ifndef STUDIOSLAB_KG_NUMERICCONFIG_H
#define STUDIOSLAB_KG_NUMERICCONFIG_H

#include "../../Core/Controller/Parameter/BuiltinParameters.h"
#include "Models/DynamicsNumericConfig.h"

namespace Slab::Models {

    using namespace Core;

    class FKGNumericConfig : public DynamicsNumericConfig {

        TPointer<RealParameter> xCenter =    New<RealParameter>(.0,   FParameterDescription{'c', "xCenter", "Center of simulation space"});
        TPointer<IntegerParameter> dimMode = New<IntegerParameter>(0, FParameterDescription{'m', "dimensions_mode",
            "Method to compute space measures:"
            "\n\t0: h=L/N (option --h is ignored)"
            "\n\t1: L=h*N (option --L is ignored)"
            "\n\t2: N=L/h (option --N is ignored)"
            "\nNote that mode 2 is not too safe if N is "
            "not Natural number."});

        TPointer<RealParameter> h = New<RealParameter>(0.009765625, FParameterDescription{'h', "Cell 1-measure. Defaults to L/N. This value is "
                                                                    "ignored unless '--mode' is properly set."});

        TPointer<RealParameter> rdt = New<RealParameter>(.1, FParameterDescription{"r_dt", "Timestep 'r' parameter such that dt=r*h."});

    public:
        explicit FKGNumericConfig(bool doRegister = true);

        FKGNumericConfig(const FKGNumericConfig &) = delete;

        [[nodiscard]] auto to_string() const -> Str override;

        [[nodiscard]] auto getn() const -> UInt override;

        [[nodiscard]] auto Getdt() const -> DevFloat;

        [[nodiscard]] auto Get_r() const -> DevFloat;

        [[nodiscard]] auto Get_xMin() const -> DevFloat;

        [[nodiscard]] auto getxMax() const -> DevFloat;

        [[nodiscard]] auto geth() const -> DevFloat;

        auto NotifyInterfaceSetupIsFinished() -> void override;


    };

} // Slab::Models::KleinGordon

#endif //STUDIOSLAB_KG_NUMERICCONFIG_H
