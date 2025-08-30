//
// Created by joao on 10/16/24.
//

#include "KG-NumericConfig.h"
#include "Core/Tools/Log.h"

namespace Slab::Models {
    using Core::Log;

    FKGNumericConfig::FKGNumericConfig(bool do_register)
            : DynamicsNumericConfig(false)
    {
        Interface->AddParameters({xCenter, rdt, dimMode, h});

        if(do_register) RegisterToManager();
    }

    auto FKGNumericConfig::Get_xMin() const -> floatt { return **xCenter - **L * .5; }

    auto FKGNumericConfig::getxMax() const -> floatt { return **xCenter + **L * .5; }

    auto FKGNumericConfig::geth() const -> floatt {
        return **h;
    }

    auto FKGNumericConfig::Getdt() const -> floatt {
        return **rdt * **h;
    }

    void FKGNumericConfig::NotifyInterfaceSetupIsFinished() {
        DynamicsNumericConfig::NotifyInterfaceSetupIsFinished();

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
    }

    Str FKGNumericConfig::to_string() const {
        const auto SEPARATOR = " ";

        return GetInterface()->ToString({"L", "N"}, SEPARATOR);
    }

    auto FKGNumericConfig::Get_r() const -> floatt { return **rdt; }

    UInt FKGNumericConfig::getn() const {
        const auto dt = this->Getdt();
        const auto t = Get_t();
        const auto n = static_cast<UInt>(round(t / dt));
        return n > 0 ? n : 1;
    }


} // Slab::Models::KleinGordon