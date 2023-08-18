#include "Base/Controller/Interface/InterfaceManager.h"
#include "NumericConfig.h"
#include "Base/Tools/Log.h"


NumericConfig::NumericConfig(bool doRegister): InterfaceOwner("Numeric Parameters,The core parameters that define the simulation per-se", 0, doRegister)
{
    interface->addParameters({N, L, xCenter, t, forceOverstepping, dt, dimMode, h});
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

auto NumericConfig::getN() const -> size_t { return **N; }
auto NumericConfig::getL() const -> floatt { return **L; }
auto NumericConfig::getxMin() const -> floatt { return **xCenter - **L * .5; }
auto NumericConfig::getxMax()  const -> floatt { return **xCenter + **L * .5; }
auto NumericConfig::gett() const -> floatt { return **t; }
auto NumericConfig::getr() const -> floatt { return **dt / **h; }
auto NumericConfig::getn() const -> Count {
    return n;
}
auto NumericConfig::geth() const -> floatt {
    return **h;
}
auto NumericConfig::getdt() const -> floatt {
    return **dt;
}

void NumericConfig::sett(Real tMax) const {
    Log::Attention() << "Command line argument '" << t->getCLName(true) << "' "
                     << "being ignored and set to " << tMax << ";" << Log::Flush;

    t->setValue(tMax);
}

bool NumericConfig::shouldForceOverstepping() const { return **forceOverstepping; }

void NumericConfig::notifyCLArgsSetupFinished() {
    InterfaceOwner::notifyCLArgsSetupFinished();

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
            *N = int(ceil( **L / **h));
            Log::Attention("NumericParams option --mode=2 => N = ceil(h*N) = ") << *N;
            break;
    }

    if(**t < 0){
        *t = **L * .5;
        Log::Attention("NumericParams") << " parameter 't' is <0. Setting to t = L/2 = " << *t;
    }

    if(**forceOverstepping)
        Log::Attention("NumericParams") << ": flag '" << forceOverstepping->getCLName(true)
        << "' is set to active: simulation time limit will be ignored.";

    if(*dt<0) {
        **dt = **h/10.0;
        Log::Attention("NumericParams") << " parameter 'dt' is <0. Setting to dt = h/10 = " << *dt;
    }

    n = PosInt(**t / **dt);
}



