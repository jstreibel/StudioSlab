//
// Created by joao on 10/16/24.
//

#include "KG-NumericConfig.h"
#include "Core/Tools/Log.h"

namespace Slab::Models {
    using Core::Log;

    KGNumericConfig::KGNumericConfig(bool do_register)
            : DynamicsNumericConfig(false)
    {
        interface->addParameters({xCenter, rdt, dimMode, h});

        if(do_register) registerToManager();
    }

    auto KGNumericConfig::getxMin() const -> floatt { return **xCenter - **L * .5; }

    auto KGNumericConfig::getxMax() const -> floatt { return **xCenter + **L * .5; }

    auto KGNumericConfig::geth() const -> floatt {
        return **h;
    }

    auto KGNumericConfig::getdt() const -> floatt {
        return **rdt * **h;
    }

    void KGNumericConfig::notifyCLArgsSetupFinished() {
        DynamicsNumericConfig::notifyCLArgsSetupFinished();

        switch (**dimMode) {
            case 0:
                *h = **L / **N;
                Log::Attention("NumericParams option --mode=0 => h = L/N = ") << *h;
                break;
            case 1:
                *L = **h * **N;
                Log::Attention("NumericParams option --mode=1 => L = h*N = ") << *L;
                break;
            case 2:
                *N = int(ceil(**L / **h));
                Log::Attention("NumericParams option --mode=2 => N = ceil(h*N) = ") << *N;
                break;
        }

        auto dt = **h * **rdt;
        n = UInt(round(**t / dt));
    }

    Str KGNumericConfig::to_string() const {
        const auto SEPARATOR = " ";

        return getInterface()->toString({"L", "N"}, SEPARATOR);
    }

    auto KGNumericConfig::getr() const -> floatt { return **rdt; }

    UInt KGNumericConfig::getn() const {
        return n;
    }


} // Slab::Models::KleinGordon