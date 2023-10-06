//
// Created by joao on 18/08/23.
//

#include "Colors.h"
#include "Utils/Utils.h"

#include <array>

#define Clamp()


namespace Styles {
    Color White = {1,1,1,1};
    Color Black = {0,0,0,1};
    Color Red = {1,0,0,1};

    Color::Color(): r(r), g(g), b(b), a(a) {

    }

    Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

    Color::Color(Color rgb, float a) : r(rgb.r), g(rgb.g), b(rgb.b), a(a) {}

    Color Color::permute(bool odd) const {
        if (odd) return {b, g, r, a};

        else return {b, r, g, a};
    }

    Color Color::inverse(bool invertAlpha) const {
        return {1-r, 1-g, 1-b, invertAlpha ? 1-a : a};
    }

    Color Color::FromBytes(Byte r, Byte g, Byte b, Byte a) {
        return {r / 255.f, g / 255.f, b / 255.f, a / 255.f};
    }

    Color Color::FromHex(Str hex) {
        const auto size = hex.length();

        if((size!=7 && size!=9) || hex[0] != '#')
            throw Exception((Str("Wrong hex color format ") + hex).c_str());

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

        return Color(r, g, b, a);
    }

    Color Color::rgb() const { return {r, g, b, -1}; }

    bool Color::operator==(const Color &rhs) const {
        fix eps = 1.e-4;
        return Common::areEqual(r, rhs.r, eps) &&
               Common::areEqual(g, rhs.g, eps) &&
               Common::areEqual(b, rhs.b, eps) &&
               Common::areEqual(a, rhs.a, eps);
    }

    std::array<Real32, 4> Color::array() const {
        return std::array<Real32, 4>({r, g, b, a});
    }


}