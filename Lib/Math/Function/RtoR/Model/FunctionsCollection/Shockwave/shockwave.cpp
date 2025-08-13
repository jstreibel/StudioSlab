#include "shockwave.h"

#include "Math/Formalism/Categories.h"



namespace Slab {

    Math::RtoR::ShockwavePhi::ShockwavePhi(DevFloat h) : h(h) {}

    DevFloat Math::RtoR::ShockwavePhi::operator()(DevFloat x) const { return 0.0; }

    Math::RtoR::ShockwaveDPhiDt::ShockwaveDPhiDt(DevFloat h) : h(h) {}

    DevFloat Math::RtoR::ShockwaveDPhiDt::operator()(DevFloat x) const { return deltaTri(x, 50.0 * h); }


}