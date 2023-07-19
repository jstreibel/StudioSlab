#include <Base/Controller/Interface/InterfaceManager.h>
#include "NumericParams.h"
#include "Common/Log/Log.h"


NumericParams::NumericParams(): InterfaceOwner("Numeric Parameters,The core parameters that define the simulation per-se", 0, true)
{
    interface->addParameters({N, L, xCenter, t, r, dimMode, h});

    Log::Info() << "Integration type is " << sizeof(Real)*8 << " bits." << Log::Flush;
}

NumericParams::NumericParams(const NumericParams &p) : n(p.n), dt(p.dt) {
    N = p.N;
    L = p.L;
    t = p.t;
    r = p.r;
    h = p.h;

    xCenter = p.xCenter;
    dimMode = p.dimMode;
}

auto NumericParams::getN() const -> size_t { return **N; }
auto NumericParams::getL() const -> floatt { return **L; }
auto NumericParams::getxLeft() const -> floatt { return **xCenter - **L*.5; }
auto NumericParams::gett() const -> floatt { return **t; }
auto NumericParams::getr() const -> floatt { return **r; }
auto NumericParams::getn() const -> size_t {
    return n;
}
auto NumericParams::geth() const -> floatt {
    return **h;
}
auto NumericParams::getdt() const -> floatt {
    return dt;
}

void NumericParams::sett(Real tMax) const {
    Log::Attention() << "Command line argument '--" << t->getCommandLineArgName(true) << "' "
                     << "being ignored and set to " << tMax << ";" << Log::Flush;

    t->setValue(tMax);
}

void NumericParams::notifyCLArgsSetupFinished() {
    InterfaceOwner::notifyCLArgsSetupFinished();

    switch (**dimMode) {
        case 0:
            *h = **L / **N;
            Log::Attention("Option --mode=0 => h = L/N = ") << *h;
            break;
        case 1:
            *L = **h * **N;
            Log::Attention("Option --mode=1 => L = h*N = ") << *L;
            break;
        case 2:
            *N = int(ceil( **L / **h));
            Log::Attention("Option --mode=2 => N = ceil(h*N) = ") << *N;
            break;
    }

    if(**t < 0) *t = **L * .5;

    dt = **r * **h;
    n = PosInt(**t / dt);
}



