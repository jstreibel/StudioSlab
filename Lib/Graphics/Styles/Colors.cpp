//
// Created by joao on 18/08/23.
//

#include "Colors.h"

#include <array>

#define Clamp()


namespace Slab::Graphics {

    FColor::FColor() = default;

    FColor::FColor(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

    FColor::FColor(FColor rgb, float a) : r(rgb.r), g(rgb.g), b(rgb.b), a(a) {}

    auto FColor::permute(bool odd) const -> FColor {
        if (odd) return {b, g, r, a};

        else return {b, r, g, a};
    }

    auto FColor::inverse(const bool invertAlpha) const -> FColor {
        return {1-r, 1-g, 1-b, invertAlpha ? 1-a : a};
    }

    FColor FColor::WithAlpha(const float alpha) const {
        return {r, g, b, alpha};
    }

    FColor FColor::WithSaturation(float s) const {
        auto hsv_color = rgb2hsv({r,g,b});
        hsv_color.s = s;

        auto rgb_color = hsv2rgb(hsv_color);
        return {
            static_cast<float>(rgb_color.r),
            static_cast<float>(rgb_color.g),
            static_cast<float>(rgb_color.b),
            a};
    }

    auto FColor::FromBytes(Byte r, Byte g, Byte b, Byte a) -> FColor {
        return {(float)r / 255.f, (float)g / 255.f, (float)b / 255.f, (float)a / 255.f};
    }

    auto FColor::FromHex(Str hex) -> FColor {
        const auto size = hex.length();

        if((size!=7 && size!=9) || hex[0] != '#')
            throw Exception(Str("Wrong hex color format ") + hex);

        unsigned int rr, gg, bb, aa=255;
        StringStream ss;
        ss << std::hex << hex.substr(1, 2);
        ss >> rr;
        ss.clear();
        ss << std::hex << hex.substr(3, 2);
        ss >> gg;
        ss.clear();
        ss << std::hex << hex.substr(5, 2);
        ss >> bb;
        if(size==9){
            ss.clear();
            ss << std::hex << hex.substr(7, 2);
            ss >> aa;
        }

        auto r = static_cast<float>(rr) / 255.0f;
        auto g = static_cast<float>(gg) / 255.0f;
        auto b = static_cast<float>(bb) / 255.0f;
        auto a = static_cast<float>(aa) / 255.0f;

        return {r, g, b, a};
    }

    auto FColor::array() const -> std::array<Real32, 4> {
        return std::array<Real32, 4>({r, g, b, a});
    }
    auto FColor::asFloat4fv() const -> const float * {
        return reinterpret_cast<const float*>(this);
    }
    auto FColor::asFloat4fv() -> float * {
        return reinterpret_cast<float*>(this);
    }
    auto FColor::rgb() const -> FColor { return {r, g, b, a}; }
    auto FColor::brg() const -> FColor { return {b, r, g, a}; }
    auto FColor::gbr() const -> FColor { return {g, b, r, a}; }

    auto FColor::bgr() const -> FColor { return {b, g, r, a}; }

    auto FColor::grb() const -> FColor { return {g, r, b, a}; }

    auto FColor::rbg() const -> FColor { return {r, b, r, a}; }

    FColor FColor::operator+(const FColor& Color) const {
        return {r+Color.r, g+Color.g, b+Color.b, a+Color.a};
    }

    FColor FColor::operator*(const Real32 v) const {
        return {v*r, v*g, v*b, v*a};
    }

    auto FColor::operator==(const FColor &rhs) const -> bool {
        fix eps = 1.e-4;
        return Common::AreEqual(r, rhs.r, eps) &&
               Common::AreEqual(g, rhs.g, eps) &&
               Common::AreEqual(b, rhs.b, eps) &&
               Common::AreEqual(a, rhs.a, eps);
    }

    bool FColor::operator!() const {
        return r<0 || g<0 || b<0 || a<0;
    }

    FColor operator*(const float lhs, const FColor& rhs) {
        return rhs*lhs;
    }
}