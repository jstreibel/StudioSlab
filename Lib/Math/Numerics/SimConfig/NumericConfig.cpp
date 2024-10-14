#include "Core/Controller/Interface/InterfaceManager.h"
#include "NumericConfig.h"
#include "Core/Tools/Log.h"


namespace Slab::Math {



    NumericConfig::NumericConfig(bool doRegister) : CLInterfaceOwner(
            "Numeric Parameters,The core parameters that define the simulation per-se", 0, doRegister) {
        interface->addParameters({N, L, xCenter, t, rdt, dimMode, h});
    }

    /*
    NumericParams::NumericParams(const NumericParams &p) : n(p.n), dt(p.dt) {
        N = p.N;
        L = p.L;
        t = p.t;
        r = p.r;
        h = p.h;

        xCenter = p.xCenter;
        dimMode = p.dimMode;
    }*/

    auto NumericConfig::getN() const -> UInt { return **N; }

    auto NumericConfig::getL() const -> floatt { return **L; }

    auto NumericConfig::getxMin() const -> floatt { return **xCenter - **L * .5; }

    auto NumericConfig::getxMax() const -> floatt { return **xCenter + **L * .5; }

    auto NumericConfig::gett() const -> floatt { return **t; }

    auto NumericConfig::getr() const -> floatt { return **rdt; }

    auto NumericConfig::getn() const -> UInt { return n; }

    auto NumericConfig::geth() const -> floatt {
        return **h;
    }

    auto NumericConfig::getdt() const -> floatt {
        return **rdt * **h;
    }

    void NumericConfig::sett(Real tMax) const {
        Log::Attention() << "Command line argument '" << t->getCommandLineArgumentName(true) << "' "
                         << "being ignored and set to " << tMax << ";" << Log::Flush;

        t->setValue(tMax);
    }

    void NumericConfig::notifyCLArgsSetupFinished() {
        CLInterfaceOwner::notifyCLArgsSetupFinished();

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


}