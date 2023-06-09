#include <Base/Controller/Interface/InterfaceManager.h>
#include "NumericParams.h"


NumericParams::NumericParams()
    : Interface("Core simulation parameters")
{
    addParameters({N, L, xCenter, t, r, dimMode, h});

    std::cout << "Integration type is " << sizeof(Real)*8 << " bits." << std::endl;
}

//NumericParams::NumericParams(const boost::program_options::variables_map& vm)
//    : N(GET("N", size_t)), L(GET("L", Real)),
//      xLeft(GET_FALLBACK("xLeft", Real, -.5*L)),
//      t(GET("t", Real)), r(GET("r", Real)), h(L/Real(N)), dt(r*h) {   }


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

void NumericParams::setup(CLVariablesMap vm) {
    Interface::setup(vm);

    switch (**dimMode) {
        case 0:
            *h = **L / **N;
            break;
        case 1:
            *L = **h * **N;
            break;
        case 2:
            *N = **L / **h;
            break;
    }

    if(**t < 0) *t = **L * .5;

    dt = **r * **h;
    n = PosInt(**t / dt);

    std::cout << "Numeric parameters are\n";

    for(auto &p : this->getParameters())
        std::cout << "\t" << std::left << std::setw(10) << p->getCommandLineArgName(true) << ": " << p << std::endl;

}

void NumericParams::sett(Real tMax) {
    std::cout << "Warning! '--" << t->getCommandLineArgName(true) << "' argument being ignored and set to "
              << tMax << ";" << std::endl;

    t->setValue(tMax);
}

