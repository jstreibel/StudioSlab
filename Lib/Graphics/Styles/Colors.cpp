//
// Created by joao on 18/08/23.
//

#include "Colors.h"

#include <array>

#define Clamp()


namespace Slab::Graphics {
    FColor White = {1,1,1,1};
    FColor Grey = {0.5,0.5,0.5,1};
    FColor LightGrey = {0.75,0.75,0.75,1};
    FColor DarkGrey = {0.25,0.25,0.25,1};
    FColor Black = {0,0,0,1};
    FColor Red = {1,0,0,1};
    FColor LapisLazuli = {38./255., 97./255., 156./255., 1};
    FColor GrassGreen = {63./255., 155./255., 11./255., 1.};
    FColor DarkGrass = FColor::FromHex("#1b4b02");

    FColor::FColor() = default;

    FColor::FColor(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

    FColor::FColor(FColor rgb, float a) : r(rgb.r), g(rgb.g), b(rgb.b), a(a) {}

    auto FColor::permute(bool odd) const -> FColor {
        if (odd) return {b, g, r, a};

        else return {b, r, g, a};
    }

    auto FColor::inverse(bool invertAlpha) const -> FColor {
        return {1-r, 1-g, 1-b, invertAlpha ? 1-a : a};
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

    auto FColor::rgb() const -> FColor { return {r, g, b, a}; }
    auto FColor::brg() const -> FColor { return {b, r, g, a}; }
    auto FColor::gbr() const -> FColor { return {g, b, r, a}; }
    auto FColor::bgr() const -> FColor { return {b, g, r, a}; }
    auto FColor::grb() const -> FColor { return {g, r, b, a}; }
    auto FColor::rbg() const -> FColor { return {r, b, r, a}; }

    auto FColor::operator==(const FColor &rhs) const -> bool {
        fix eps = 1.e-4;
        return Common::AreEqual(r, rhs.r, eps) &&
               Common::AreEqual(g, rhs.g, eps) &&
               Common::AreEqual(b, rhs.b, eps) &&
               Common::AreEqual(a, rhs.a, eps);
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

    FColor FColor::operator*(const Real32 v) const {
        return {v*r, v*g, v*b, v*a};
    }
}