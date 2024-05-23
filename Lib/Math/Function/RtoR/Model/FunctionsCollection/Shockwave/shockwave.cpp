#include "shockwave.h"

#include "Math/Formalism/Categories.h"



namespace Slab {

    Math::RtoR::ShockwavePhi::ShockwavePhi(Real h) : h(h) {}

    Real Math::RtoR::ShockwavePhi::operator()(Real x) const { return 0.0; }

    Math::RtoR::ShockwaveDPhiDt::ShockwaveDPhiDt(Real h) : h(h) {}

    Real Math::RtoR::ShockwaveDPhiDt::operator()(Real x) const { return deltaTri(x, 50.0 * h); }


}