//
// Created by joao on 18/08/23.
//

#include "Colors.h"

#include <array>

#define Clamp()


namespace Slab::Graphics {
    Color White = {1,1,1,1};
    Color Black = {0,0,0,1};
    Color Red = {1,0,0,1};

    Color::Color(): r(r), g(g), b(b), a(a) {

    }

    Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

    Color::Color(Color rgb, float a) : r(rgb.r), g(rgb.g), b(rgb.b), a(a) {}

    auto Color::permute(bool odd) const -> Color {
        if (odd) return {b, g, r, a};

        else return {b, r, g, a};
    }

    auto Color::inverse(bool invertAlpha) const -> Color {
        return {1-r, 1-g, 1-b, invertAlpha ? 1-a : a};
    }

    auto Color::FromBytes(Byte r, Byte g, Byte b, Byte a) -> Color {
        return {(float)r / 255.f, (float)g / 255.f, (float)b / 255.f, (float)a / 255.f};
    }

    auto Color::FromHex(Str hex) -> Color {
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

    auto Color::rgb() const -> Color { return {r, g, b, a}; }
    auto Color::brg() const -> Color { return {b, r, g, a}; }
    auto Color::gbr() const -> Color { return {g, b, r, a}; }
    auto Color::bgr() const -> Color { return {b, g, r, a}; }
    auto Color::grb() const -> Color { return {g, r, b, a}; }
    auto Color::rbg() const -> Color { return {r, b, r, a}; }

    auto Color::operator==(const Color &rhs) const -> bool {
        fix eps = 1.e-4;
        return Common::AreEqual(r, rhs.r, eps) &&
               Common::AreEqual(g, rhs.g, eps) &&
               Common::AreEqual(b, rhs.b, eps) &&
               Common::AreEqual(a, rhs.a, eps);
    }

    auto Color::array() const -> std::array<Real32, 4> {
        return std::array<Real32, 4>({r, g, b, a});
    }

    auto Color::asFloat4fv() const -> const float * {
        return reinterpret_cast<const float*>(this);
    }

    auto Color::asFloat4fv() -> float * {
        return reinterpret_cast<float*>(this);
    }
}