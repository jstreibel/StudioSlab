//
// Created by joao on 24/09/2019.
//

#include "Categories.h"
#include "Core/Tools/Log.h"

#include <utility>


namespace Slab::Math {

    void TestRoundingAndRationals() {
        using namespace Slab::Core;

        for (int i = 0; i < 1000; ++i) {
            auto oVal = (DevFloat) i / 7.;
            auto lVal = Common::RoundToMostSignificantDigits(oVal, 2);
            Rational rat(lVal, 1e-9);
            auto color = rat.denominator != 0 ? Log::FGBlack + Log::BGGreen : "";
            Log::Info() << i << ": " << oVal << "   " << color << lVal << " --> " << rat << Log::ResetFormatting
                        << Log::Flush;
        }
        exit(0);
    }

    OStream &operator<<(OStream &stream, const Real2D &x) {
        return stream << "(" << x.x << "," << x.y << ")";
    }

    Rational floatToRational(const DevFloat floatValue, DevFloat epsilon = 1e-9, int maxIter = 1000) {
        Int a;
        DevFloat c;
        Rational r1 = {1, 0};
        Rational r2 = {0, 1};
        Rational r;
        DevFloat x = floatValue;

        while (maxIter--) {
            a = static_cast<Int>(x);
            c = x - a;

            r.numerator = a * r1.numerator + r2.numerator;
            r.denominator = a * r1.denominator + r2.denominator;

            fix val = DevFloat(r.numerator) / DevFloat(r.denominator);
            fix err = std::abs(val - floatValue);
            if (err < epsilon)
                return r;

            x = 1.0 / c;
            r2 = r1;
            r1 = r;
        }

        return {0, 0}; // Fallback, should not happen for reasonable inputs and epsilon
    }

    Rational::Rational() = default;

    Rational::Rational(const Int numerator, const Int denominator)
            : numerator(numerator), denominator(denominator) {}

    Rational::Rational(const DevFloat val, const DevFloat epsilon, const Int maxIter)
            : Rational(floatToRational(val, epsilon, maxIter)) {}

    Str Rational::ToString() const {
        if (denominator == 1) return ToStr(numerator);

        return ToStr(numerator) + "/" + ToStr(denominator);
    }

    Unit::Unit(const Unit &unit) : Unit(unit.sym, unit.baseValue) {}

    Unit::Unit(Str symbol, const DevFloat val, unsigned maxNumerator, unsigned maxDenominator)
            : sym(std::move(symbol)), baseValue(val), maxNumerator(maxNumerator), maxDenominator(maxDenominator) {}

    Str Unit::ToString() const {
        return Category::ToString();
    }

    Str Unit::operator()(const DevFloat &val, const CountType rounding) const {
        if (Common::AreEqual(baseValue, 1) && sym.empty()) return ToStr(val, static_cast<int>(rounding));

        fix norm_val = val / baseValue;
        fix norm_val_rounded = Common::RoundToMostSignificantDigits(norm_val, static_cast<int>(rounding));

        const Rational rational(norm_val_rounded);

        if (std::abs(rational.numerator) > maxNumerator
            || std::abs(rational.denominator) > maxDenominator)
            return ToStr(norm_val, static_cast<int>(rounding)) + sym;

        auto unitVal = sym;
        if (std::abs(rational.numerator) != 1) unitVal = ToStr(rational.numerator) + unitVal;
        if (std::abs(rational.denominator) != 1) unitVal = unitVal + "/" + ToStr(rational.denominator);

        return unitVal;
    }

    DevFloat Unit::value() const {
        return baseValue;
    }

    Str Unit::symbol() const {
        return sym;
    }


}