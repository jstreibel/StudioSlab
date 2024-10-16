//
// Created by joao on 10/16/24.
//

#include "KGNumericConfig.h"
#include "Core/Tools/Log.h"

namespace Slab::Models {
    using Core::Log;

    KGNumericConfig::KGNumericConfig(bool doRegister)
            : Math::NumericConfig(doRegister, {N, L, xCenter, t, rdt, dimMode, h})
    {

    }

    auto KGNumericConfig::getN() const -> UInt { return **N; }

    auto KGNumericConfig::getL() const -> floatt { return **L; }

    auto KGNumericConfig::getxMin() const -> floatt { return **xCenter - **L * .5; }

    auto KGNumericConfig::getxMax() const -> floatt { return **xCenter + **L * .5; }

    auto KGNumericConfig::gett() const -> floatt { return **t; }

    auto KGNumericConfig::getr() const -> floatt { return **rdt; }

    auto KGNumericConfig::getn() const -> UInt { return n; }

    auto KGNumericConfig::geth() const -> floatt {
        return **h;
    }

    auto KGNumericConfig::getdt() const -> floatt {
        return **rdt * **h;
    }

    void KGNumericConfig::sett(Real tMax) const {
        Log::Attention() << "Command line argument '" << t->getCommandLineArgumentName(true) << "' "
                         << "being ignored and set to " << tMax << ";" << Log::Flush;

        t->setValue(tMax);
    }

    void KGNumericConfig::notifyCLArgsSetupFinished() {
        Math::NumericConfig::notifyCLArgsSetupFinished();

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

        if (**t < 0) {
            *t = **L * .5;
            Log::Attention("NumericParams") << " parameter 't' is <0. Setting to t = L/2 = " << *t;
        }

        if (*rdt < 0) {
            *rdt = .1;
            auto dt = **h * **rdt;
            Log::Attention("NumericParams") << " parameter 'r_dt' is <0. Setting to r_dt=0.1 → dt = h/10 = " << dt;
        }

        auto dt = **h * **rdt;
        n = UInt(round(**t / dt));
    }

    Str KGNumericConfig::to_string() const {
        const auto SEPARATOR = " ";

        return getInterface()->toString({"L", "N"}, SEPARATOR);
    }


} // Slab::Models::KleinGordon