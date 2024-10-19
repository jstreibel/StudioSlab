//

#include "R2toRNumericFunction.h"

// don't touch:
#define PERIODIC 1
#define OD_NAN 2
#define OD_ZERO 3
#define ROUND_ROUND(x) round(x)
#define FLOOR_ROUND(x) floor(.5f + (x))

// ok touch:
#define OFF_DOMAIN_BEHAVIOR PERIODIC
#define ROUND(x) ROUND_ROUND((x))

namespace Slab::Math {

//
// Created by joao on 30/09/2019.
    R2toR::NumericFunction::NumericFunction(UInt N, UInt M, Real xMin, Real yMin, Real hx, Real hy, Device dev)
            : NumericFunctionBase(DimensionMetaData({N, M}, {hx, hy}), dev), N(N), M(M), xMin(xMin),
              xMax(xMin + (Real) N * hx), yMin(yMin), yMax(yMin + (Real) M * hy), hx(hx), hy(hy) {}

    // R2toR::NumericFunction::NumericFunction(const NumericConfig &p, Device dev)
    //         : R2toR::NumericFunction(p.getN(), p.getN(), p.getxMin(), p.getxMin(), p.geth(), p.geth(), dev) {}

    Real R2toR::NumericFunction::operator()(Real2D r) const {
        const Real Lx = xMax - xMin;
        const Real Ly = yMax - yMin;
        fix x = r.x;
        fix y = r.y;

        fix nReal = (N - 1) * (x - xMin) / Lx;
        fix mReal = (M - 1) * (y - yMin) / Ly;

        int n = ROUND(nReal);
        int m = ROUND(mReal);


        // TODO: responsabilizar classes filhas por acesso fora do dominio.

#if OFF_DOMAIN_BEHAVIOR == OD_NAN
        if(n<0 || m<0 || n>N-1 || m>M-1) return NaN;
#elif OFF_DOMAIN_BEHAVIOR == OD_ZERO
        if(n<0 || m<0 || n>N-1 || m>M-1) return OD_ZERO;
#elif OFF_DOMAIN_BEHAVIOR == PERIODIC
        if (n < 0) n = N - abs(n) % N;
        else if (n > N - 1) n = n % N;
        if (m < 0) m = M - abs(m) % M;
        else if (m > M - 1) m = m % M;
#endif

        return At(n, m);
    }

    auto R2toR::NumericFunction::operator()(Real2D r) -> Real & {
        const Real Lx = xMax - xMin;
        const Real Ly = yMax - yMin;
        fix x = r.x;
        fix y = r.y;

        fix nReal = (N - 1) * (x - xMin) / Lx;
        fix mReal = (M - 1) * (y - yMin) / Ly;

        int n = ROUND(nReal);
        int m = ROUND(mReal);

        return At(n, m);
    }

    UInt R2toR::NumericFunction::getN() const { return N; }

    UInt R2toR::NumericFunction::getM() const { return M; }

    R2toR::Domain R2toR::NumericFunction::getDomain() const { return {xMin, xMax, yMin, yMax}; }

    auto R2toR::NumericFunction::diff(int dim, int n, int m) const -> Real {
        if (n < 0 || n > N - 1 || m < 0 || m > M - 1) return .0;

        getSpace().syncHost();
        auto &X = getSpace().getHostData();
        // int j = n + m*N;

        if (dim == 0) {
            const Real s = .5 / hx;
            fix LEFT = n==0 ? At(N-1, m) : At(n-1, m);
            fix RIGHT = n==N-1 ? At(0, m) : At(n + 1, m);
            return s * (RIGHT - LEFT);
        } else if (dim == 1) {
            Real s = .5 / hy;
            auto TOP_m = m+1;
            auto BOTTOM_m = m-1;
            if(m==M-1) {
                TOP_m = (int)M-1;
                s *= 2;
            } else if(m==0){
                BOTTOM_m = 0;
                s *= 2;
            }

            fix TOP = At(n, TOP_m);
            fix BOTTOM = At(n, BOTTOM_m);

            return s * (TOP - BOTTOM);
        } else
            NOT_IMPLEMENTED;
    }

    Real R2toR::NumericFunction::diff(int dim, Real2D x) const {
        const Real Lx = xMax - xMin;
        const Real Ly = yMax - yMin;
        int n = int((N - 1) * (x.x - xMin) / Lx);
        int m = int((M - 1) * (x.y - yMin) / Ly);

        return diff(dim, n, m);
    }

    R2toR::Function_ptr R2toR::NumericFunction::diff(int n) const {
        auto my_diff = DynamicPointerCast<R2toR::NumericFunction>(Clone());

        my_diff->change_data_name(Map<int, Str>{{0, "𝜕ₓ"}, {1, "𝜕ₜ"}}[n] + this->get_data_name());

        for(auto i=0; i<N; ++i)
            for(auto j=0; j<M; ++j)
                my_diff->At(i, j) = (*this).diff(n, i, j);

        return my_diff;
    }

    Str R2toR::NumericFunction::generalName() const {
        return Base::NumericFunction<Real2D, Real>::generalName() + " 2D " + (dev == GPU ? "GPU" : "CPU");
    }

    bool R2toR::NumericFunction::domainContainsPoint(Real2D x) const {
        return x.x >= xMin && x.x <= xMax && x.y >= yMin && x.y <= yMax;
    }

    Real R2toR::NumericFunction::max() const {
        return getSpace().getHostData(true).max();
    }

    Real R2toR::NumericFunction::min() const {
        return getSpace().getHostData(true).min();
    }

}