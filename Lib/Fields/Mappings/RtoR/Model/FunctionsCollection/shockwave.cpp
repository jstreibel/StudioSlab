#include "shockwave.h"

#include <Phys/Toolset/Categories.h>


RtoR::ShockwavePhi::ShockwavePhi(Real h) : h(h) { }
Real RtoR::ShockwavePhi::operator()(Real x) const { return 0.0; }

RtoR::ShockwaveDPhiDt::ShockwaveDPhiDt(Real h) : h(h) { }
Real RtoR::ShockwaveDPhiDt::operator()(Real x) const { return delta(x, 50.0*h); }
