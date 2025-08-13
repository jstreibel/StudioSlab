//
// Created by joao on 8/3/25.
//

#include "SGPlaneWave.h"

namespace Slab::Math::RtoR {

    constexpr DevFloat ModeCount = 15;

    FSignumGordonPlaneWave::FSignumGordonPlaneWave(const DevFloat Q, const DevFloat k) : Q(Q), k(k) { }

    FSignumGordonPlaneWave::FSignumGordonPlaneWave(const FSignumGordonPlaneWave& other): RtoR::Function(other),
        Q(other.Q),
        k(other.k)
    {
    }

    FSignumGordonPlaneWave::FSignumGordonPlaneWave(FSignumGordonPlaneWave&& other) noexcept: RtoR::Function(std::move(other)),
        Q(other.Q),
        k(other.k)
    {
    }

    FSignumGordonPlaneWave& FSignumGordonPlaneWave::operator=(const FSignumGordonPlaneWave& other)
    {
        if (this == &other)
            return *this;
        RtoR::Function::operator =(other);
        Q = other.Q;
        k = other.k;
        return *this;
    }

    FSignumGordonPlaneWave& FSignumGordonPlaneWave::operator=(FSignumGordonPlaneWave&& other) noexcept
    {
        if (this == &other)
            return *this;
        RtoR::Function::operator =(std::move(other));
        Q = other.Q;
        k = other.k;
        return *this;
    }

    DevFloat FSignumGordonPlaneWave::operator()(const DevFloat x) const
    {
        // Q=Ak²=Aω²-4/π
        fix A = Q/(k*k);
        // fix ω = sqrt((Q + 4*M_1_PI) / A);

        DevFloat Sum = 0;
        DevFloat Sign = 1;
        for (Int i=0; i<ModeCount; ++i)
        {
            fix n = 2*i + 1;

            fix inv_n³ = 1.0/(n*n*n);

            Sum += Sign * inv_n³ * cos(n*k*x);

            Sign *= -1;
        }

        return A*Sum;
    }

    FSignumGordonPlaneWave_TimeDerivative::FSignumGordonPlaneWave_TimeDerivative(const DevFloat Q, const DevFloat k)
    : Q(Q), k(k) { }

    FSignumGordonPlaneWave_TimeDerivative::FSignumGordonPlaneWave_TimeDerivative(
        const FSignumGordonPlaneWave_TimeDerivative& other): RtoR::Function(other),
                                                             Q(other.Q),
                                                             k(other.k)
    {
    }

    FSignumGordonPlaneWave_TimeDerivative::FSignumGordonPlaneWave_TimeDerivative(
        FSignumGordonPlaneWave_TimeDerivative&& other) noexcept: RtoR::Function(std::move(other)),
                                                                 Q(other.Q),
                                                                 k(other.k)
    {
    }

    FSignumGordonPlaneWave_TimeDerivative& FSignumGordonPlaneWave_TimeDerivative::operator=(
        const FSignumGordonPlaneWave_TimeDerivative& other)
    {
        if (this == &other)
            return *this;
        RtoR::Function::operator =(other);
        Q = other.Q;
        k = other.k;
        return *this;
    }

    FSignumGordonPlaneWave_TimeDerivative& FSignumGordonPlaneWave_TimeDerivative::operator=(
        FSignumGordonPlaneWave_TimeDerivative&& other) noexcept
    {
        if (this == &other)
            return *this;
        RtoR::Function::operator =(std::move(other));
        Q = other.Q;
        k = other.k;
        return *this;
    }

    DevFloat FSignumGordonPlaneWave_TimeDerivative::operator()(DevFloat x) const
    {
        // Q=Ak²=Aω²-4/π
        fix A = Q/(k*k);
        fix ω = sqrt((Q + 4*M_1_PI) / A);

        DevFloat Sum = 0;
        DevFloat Sign = 1.0;
        for (Int i=0; i<ModeCount; ++i)
        {
            fix n = 2*i + 1;

            fix inv_n² = 1.0/(n*n);

            Sum += Sign * inv_n² * sin(n*k*x);

            Sign *= -1;
        }

        return A*ω*Sum;
    }
} // Slab::Math::RtoR