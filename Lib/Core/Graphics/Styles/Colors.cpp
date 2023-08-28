//
// Created by joao on 18/08/23.
//

#include "Colors.h"

#define Clamp()

namespace Styles {
    Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

    Color::Color(Color rgb, float a) : r(rgb.r), g(rgb.g), b(rgb.b), a(a) {}

    Color Color::permute(bool odd) const {
        if (odd) return {g, r, b, a};

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
            throw Str("Wrong hex color format ") + hex;

        unsigned int rr, gg, bb, aa=255;
        std::stringstream ss;
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

    std::vector<Real> Color::toVector() { return {r, g, b}; }

    Color Color::rgb() const { return {r, g, b, -1}; }

    bool Color::operator==(const Color &rhs) const {
        static float eps = 1.e-4;
        return r > rhs.r-eps  && r < rhs.r+eps &&
               g > rhs.g-eps  && g < rhs.g+eps &&
               b > rhs.b-eps  && b < rhs.b+eps &&
               a > rhs.a-eps  && a < rhs.a+eps;
    }



}