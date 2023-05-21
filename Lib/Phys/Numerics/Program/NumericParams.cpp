#include <Base/Controller/Interface/InterfaceManager.h>
#include "NumericParams.h"


NumericParams::NumericParams()
    : Interface("Core simulation parameters")
{
    addParameters({&N, &L, &xCenter, &t, &r});
}

//NumericParams::NumericParams(const boost::program_options::variables_map& vm)
//    : N(GET("N", size_t)), L(GET("L", double)),
//      xLeft(GET_FALLBACK("xLeft", double, -.5*L)),
//      t(GET("t", double)), r(GET("r", double)), h(L/double(N)), dt(r*h) {   }


auto NumericParams::getN() const -> size_t { return N.value(); }
auto NumericParams::getL() const -> floatt { return L.value(); }
auto NumericParams::getxLeft() const -> floatt { return *xCenter - *L*.5; }
auto NumericParams::gett() const -> floatt { return  t.value(); }
auto NumericParams::getr() const -> floatt { return r.value(); }
auto NumericParams::getn() const -> size_t {
    return n;
}
auto NumericParams::geth() const -> floatt {
    return h;
}
auto NumericParams::getdt() const -> floatt {
    return dt;
}

void NumericParams::setup(CLVariablesMap vm) {
    Interface::setup(vm);

    h = *L / *N;
    dt = *r * h;
    n = PosInt(*t / dt);
}

