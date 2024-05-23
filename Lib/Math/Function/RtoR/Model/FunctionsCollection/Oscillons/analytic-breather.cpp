#include "analytic-breather.h"
#include "Math/Toolset/NativeFunctions.h"

/* (*Basic partial solutions for 0<t'<1/2 *) */

namespace Slab::Math::RtoR {

    inline floatt vphiC(floatt t, floatt x, floatt v) {
        return .125 * (sqr(1. + v - 2. * x) - 4. * t * (1. + v - 2. * v * x) + 4. * (2 - v * v) * t * t) / (1. - v * v);
    }

    inline floatt vphiL1(floatt t, floatt x, floatt v) { return .5 * t * t - (t * x) / (1. + v); }

    inline floatt vphiL2(floatt t, floatt x, floatt v) { return -.5 * sqr(x - v * t) / (1. - v * v); }

    inline floatt vphiL3(floatt t, floatt x, floatt v) { return .5 * (t - .5) * (t + .5 + (2. * x - 1.) / (1. - v)); }

    inline floatt vphiR1(floatt t, floatt x, floatt v) { return vphiL1(t, 1. - x, -v); }

    inline floatt vphiR2(floatt t, floatt x, floatt v) { return vphiL2(t, 1. - x, -v); }

    inline floatt vphiR3(floatt t, floatt x, floatt v) { return vphiL3(t, 1. - x, -v); }

/* (*Derivatives \partial_t and \partial_x of basic partial solutions*) */
    inline floatt dtvphiC(floatt t, floatt x, floatt v) {
        return (t * (2. - v * v) - .5 * (1. + v - 2. * v * x)) / (1. - v * v);
    }

    inline floatt dxvphiC(floatt t, floatt x, floatt v) { return (t * v - .5 * (1. + v - 2. * x)) / (1. - v * v); }

    inline floatt dtvphiL1(floatt t, floatt x, floatt v) { return t - x / (1. + v); }

    inline floatt dxvphiL1(floatt t, floatt x, floatt v) {
        (void) x;
        return -t / (1. + v);
    }

    inline floatt dtvphiL2(floatt t, floatt x, floatt v) { return v * (-t * v + x) / (1. - v * v); } /**/
    inline floatt dxvphiL2(floatt t, floatt x, floatt v) { return -(-t * v + x) / (1. - v * v); } /**/
    inline floatt dtvphiL3(floatt t, floatt x, floatt v) { return (.5 + t * (-1. + v) - x) / (-1. + v); }

    inline floatt dxvphiL3(floatt t, floatt x, floatt v) {
        (void) x;
        return (-.5 + t) / (1. - v);
    }

    inline floatt dtvphiR1(floatt t, floatt x, floatt v) { return t - (1. - x) / (1. - v); }

    inline floatt dxvphiR1(floatt t, floatt x, floatt v) {
        (void) x;
        return t / (1. - v);
    }

    inline floatt dtvphiR2(floatt t, floatt x, floatt v) { return -v * (1. + t * v - x) / (1. - v * v); }

    inline floatt dxvphiR2(floatt t, floatt x, floatt v) { return (1. + t * v - x) / (1. - v * v); }

    inline floatt dtvphiR3(floatt t, floatt x, floatt v) { return (.5 + t + t * v - x) / (1. + v); }

    inline floatt dxvphiR3(floatt t, floatt x, floatt v) {
        (void) x;
        return (.5 - t) / (1. + v);
    }

/* (* Step funstions to cutt-off the partial solutions *) */
    inline floatt H(floatt z) {
        return UnitStep(z);
    }

    inline floatt PiC(floatt t, floatt x, floatt v) {
        return H(x - t) * H(-x - t + 1.) * H(x + t - .5 * (1. + v)) * H(-x + t + .5 * (1. + v));
    }

    inline floatt PiL1(floatt t, floatt x, floatt v) {
        return H(t) * H(x - t) * H(-x - t + .5 * (1. + v));
    }

    inline floatt PiL2(floatt t, floatt x, floatt v) {
        return H(-x + t) * H(-x - t + .5 * (1. + v)) * H(x - v * t);
    }

    inline floatt PiL3(floatt t, floatt x, floatt v) {
        return H(-t + .5) * H(-x + t) * H(+x + t - .5 * (1. + v));
    }

    inline floatt PiR1(floatt t, floatt x, floatt v) {
        return PiL1(t, 1. - x, -v);
    }

    inline floatt PiR2(floatt t, floatt x, floatt v) {
        return PiL2(t, 1. - x, -v);
    }

    inline floatt PiR3(floatt t, floatt x, floatt v) {
        return PiL3(t, 1. - x, -v);
    }

/* (* Periodic functons of time allows to introduce priodicity for the breather
    solution *) */

/* TODO: this is unnecessarily super-slow. Optimize.*/
    inline floatt tau(floatt xi) {
        // return pi_1*asin(ABS<floatt>(sin(pi*xi)));
        return abs(xi - 1. - floor(xi - .5));
    }

    inline floatt sigma(floatt xi) {
        //return SIGN<floatt>(sin(2.*pi*xi));
        return 1. - 2. * floor(.5 - (floor(xi) - xi));
    }

/* (* The Breather field *) */
    floatt phiC(floatt t, floatt x, floatt v) { return sigma(t) * PiC(tau(t), x, v) * vphiC(tau(t), x, v); }

    floatt phiL1(floatt t, floatt x, floatt v) { return sigma(t) * PiL1(tau(t), x, v) * vphiL1(tau(t), x, v); }

    floatt phiR1(floatt t, floatt x, floatt v) { return sigma(t) * PiR1(tau(t), x, v) * vphiR1(tau(t), x, v); }

    floatt phiL2(floatt t, floatt x, floatt v) { return sigma(t) * PiL2(tau(t), x, v) * vphiL2(tau(t), x, v); }

    floatt phiR2(floatt t, floatt x, floatt v) { return sigma(t) * PiR2(tau(t), x, v) * vphiR2(tau(t), x, v); }

    floatt phiL3(floatt t, floatt x, floatt v) { return sigma(t) * PiL3(tau(t), x, v) * vphiL3(tau(t), x, v); }

    floatt phiR3(floatt t, floatt x, floatt v) { return sigma(t) * PiR3(tau(t), x, v) * vphiR3(tau(t), x, v); }

    floatt getPhiValue(floatt t, floatt x, floatt v) {
        return phiC(t, x, v) +
               phiL1(t, x, v) +
               phiL2(t, x, v) +
               phiL3(t, x, v) +
               phiR1(t, x, v) +
               phiR2(t, x, v) +
               phiR3(t, x, v);
    }

/* (* Temporal derivative of the breather field *) */
    floatt dphiC(floatt t, floatt x, floatt v) { return PiC(tau(t), x, v) * dtvphiC(tau(t), x, v); }

    floatt dphiL1(floatt t, floatt x, floatt v) { return PiL1(tau(t), x, v) * dtvphiL1(tau(t), x, v); }

    floatt dphiR1(floatt t, floatt x, floatt v) { return dphiL1(t, 1 - x, -v); }

    floatt dphiL2(floatt t, floatt x, floatt v) { return PiL2(tau(t), x, v) * dtvphiL2(tau(t), x, v); }

    floatt dphiR2(floatt t, floatt x, floatt v) { return dphiL2(t, 1 - x, -v); }

    floatt dphiL3(floatt t, floatt x, floatt v) { return PiL3(tau(t), x, v) * dtvphiL3(tau(t), x, v); }

    floatt dphiR3(floatt t, floatt x, floatt v) { return dphiL3(t, 1 - x, -v); }

    floatt getdPhidtValue(floatt t, floatt x, floatt v) {
        //std::cout << "Dafuk?" << std::endl;
        return dphiC(t, x, v) +
               dphiL1(t, x, v) +
               dphiL2(t, x, v) +
               dphiL3(t, x, v) +
               dphiR1(t, x, v) +
               dphiR2(t, x, v) +
               dphiR3(t, x, v);
    }

/* (* BREATHER IN UNIFORM MOTION *) */
#define gamma(u) (1./sqrt(1.-(u)*(u)))
#define xi   (gamma(u)*(t-u*x))
#define zeta (gamma(u)*(x-u*t))

    floatt psiC(floatt t, floatt x, floatt v, floatt u) { return phiC(xi, zeta, v); }

    floatt psiL1(floatt t, floatt x, floatt v, floatt u) { return phiL1(xi, zeta, v); }

    floatt psiR1(floatt t, floatt x, floatt v, floatt u) { return phiR1(xi, zeta, v); }

    floatt psiL2(floatt t, floatt x, floatt v, floatt u) { return phiL2(xi, zeta, v); }

    floatt psiR2(floatt t, floatt x, floatt v, floatt u) { return phiR2(xi, zeta, v); }

    floatt psiL3(floatt t, floatt x, floatt v, floatt u) { return phiL3(xi, zeta, v); }

    floatt psiR3(floatt t, floatt x, floatt v, floatt u) { return phiR3(xi, zeta, v); }


    floatt xiC(floatt t, floatt x, floatt v, floatt u) { return phiC(xi, -zeta, v); }

    floatt xiL1(floatt t, floatt x, floatt v, floatt u) { return phiL1(xi, -zeta, v); }

    floatt xiR1(floatt t, floatt x, floatt v, floatt u) { return phiR1(xi, -zeta, v); }

    floatt xiL2(floatt t, floatt x, floatt v, floatt u) { return phiL2(xi, -zeta, v); }

    floatt xiR2(floatt t, floatt x, floatt v, floatt u) { return phiR2(xi, -zeta, v); }

    floatt xiL3(floatt t, floatt x, floatt v, floatt u) { return phiL3(xi, -zeta, v); }

    floatt xiR3(floatt t, floatt x, floatt v, floatt u) { return phiR3(xi, -zeta, v); }


    floatt dpsiC(floatt t, floatt x, floatt v, floatt u) {
        return gamma(u) * PiC(tau(xi), zeta, v) * (dtvphiC(tau(xi), zeta, v) -
                                                   u * sigma(xi) * dxvphiC(tau(xi), zeta, v));
    }

    floatt dpsiL1(floatt t, floatt x, floatt v, floatt u) {
        return gamma(u) * PiL1(tau(xi), zeta, v) * (dtvphiL1(tau(xi), zeta, v) -
                                                    u * sigma(xi) * dxvphiL1(tau(xi), zeta, v));
    }

    floatt dpsiR1(floatt t, floatt x, floatt v, floatt u) {
        return gamma(u) * PiR1(tau(xi), zeta, v) * (dtvphiR1(tau(xi), zeta, v) -
                                                    u * sigma(xi) * dxvphiR1(tau(xi), zeta, v));
    }

    floatt dpsiL2(floatt t, floatt x, floatt v, floatt u) {
        return gamma(u) * PiL2(tau(xi), zeta, v) * (dtvphiL2(tau(xi), zeta, v) -
                                                    u * sigma(xi) * dxvphiL2(tau(xi), zeta, v));
    }

    floatt dpsiR2(floatt t, floatt x, floatt v, floatt u) {
        return gamma(u) * PiR2(tau(xi), zeta, v) * (dtvphiR2(tau(xi), zeta, v) -
                                                    u * sigma(xi) * dxvphiR2(tau(xi), zeta, v));
    }

    floatt dpsiL3(floatt t, floatt x, floatt v, floatt u) {
        return gamma(u) * PiL3(tau(xi), zeta, v) * (dtvphiL3(tau(xi), zeta, v) -
                                                    u * sigma(xi) * dxvphiL3(tau(xi), zeta, v));
    }

    floatt dpsiR3(floatt t, floatt x, floatt v, floatt u) {
        return gamma(u) * PiR3(tau(xi), zeta, v) * (dtvphiR3(tau(xi), zeta, v) -
                                                    u * sigma(xi) * dxvphiR3(tau(xi), zeta, v));
    }


    floatt dxiC(floatt t, floatt x, floatt v, floatt u) {
        return gamma(u) * PiC(tau(xi), -zeta, v) * (dtvphiC(tau(xi), -zeta, v) +
                                                    u * sigma(xi) * dxvphiC(tau(xi), -zeta, v));
    }

    floatt dxiL1(floatt t, floatt x, floatt v, floatt u) {
        return gamma(u) * PiL1(tau(xi), -zeta, v) * (dtvphiL1(tau(xi), -zeta, v) +
                                                     u * sigma(xi) * dxvphiL1(tau(xi), -zeta, v));
    }

    floatt dxiR1(floatt t, floatt x, floatt v, floatt u) {
        return gamma(u) * PiR1(tau(xi), -zeta, v) * (dtvphiR1(tau(xi), -zeta, v) +
                                                     u * sigma(xi) * dxvphiR1(tau(xi), -zeta, v));
    }

    floatt dxiL2(floatt t, floatt x, floatt v, floatt u) {
        return gamma(u) * PiL2(tau(xi), -zeta, v) * (dtvphiL2(tau(xi), -zeta, v) +
                                                     u * sigma(xi) * dxvphiL2(tau(xi), -zeta, v));
    }

    floatt dxiR2(floatt t, floatt x, floatt v, floatt u) {
        return gamma(u) * PiR2(tau(xi), -zeta, v) * (dtvphiR2(tau(xi), -zeta, v) +
                                                     u * sigma(xi) * dxvphiR2(tau(xi), -zeta, v));
    }

    floatt dxiL3(floatt t, floatt x, floatt v, floatt u) {
        return gamma(u) * PiL3(tau(xi), -zeta, v) * (dtvphiL3(tau(xi), -zeta, v) +
                                                     u * sigma(xi) * dxvphiL3(tau(xi), -zeta, v));
    }

    floatt dxiR3(floatt t, floatt x, floatt v, floatt u) {
        return gamma(u) * PiR3(tau(xi), -zeta, v) * (dtvphiR3(tau(xi), -zeta, v) +
                                                     u * sigma(xi) * dxvphiR3(tau(xi), -zeta, v));
    }

/* (* A complete breather *) */
    floatt psi(floatt t, floatt x, floatt v, floatt u) {
        return psiC(t, x, v, u) +
               psiL1(t, x, v, u) +
               psiR1(t, x, v, u) +
               psiL2(t, x, v, u) +
               psiR2(t, x, v, u) +
               psiL3(t, x, v, u) +
               psiR3(t, x, v, u);
    }

    floatt dpsi(floatt t, floatt x, floatt v, floatt u) {
        return dpsiC(t, x, v, u)
               + dpsiL1(t, x, v, u)
               + dpsiR1(t, x, v, u)
               + dpsiL2(t, x, v, u)
               + dpsiR2(t, x, v, u)
               + dpsiL3(t, x, v, u)
               + dpsiR3(t, x, v, u);
    }

    floatt chi(floatt t, floatt x, floatt v, floatt u) {
        return xiC(t, x, v, u) +
               xiL1(t, x, v, u) +
               xiR1(t, x, v, u) +
               xiL2(t, x, v, u) +
               xiR2(t, x, v, u) +
               xiL3(t, x, v, u) +
               xiR3(t, x, v, u);
    }

    floatt dchi(floatt t, floatt x, floatt v, floatt u) {
        return dxiC(t, x, v, u) +
               dxiL1(t, x, v, u) +
               dxiR1(t, x, v, u) +
               dxiL2(t, x, v, u) +
               dxiR2(t, x, v, u) +
               dxiL3(t, x, v, u) +
               dxiR3(t, x, v, u);
    }

}