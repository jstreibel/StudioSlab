//
// Created by joao on 19/09/2021.
//

#ifndef SLAB_COLOR_H
#define SLAB_COLOR_H

#include <cmath>
#include "Utils/Types.h"

namespace Slab::Graphics {
    struct Color
    {
    public:
        float r, g, b, a;

        Color();
        Color(float r, float g, float b, float a=1.);
        Color(Color rgb, float a);
        Color(const Color &c) = default;


        auto permute(bool odd=false) const -> Color;
        auto inverse(bool invertAlpha=false) const -> Color;

        static auto FromBytes(Byte r, Byte g, Byte b, Byte a=0xff) -> Color;
        static auto FromHex(Str hex) -> Color;

        auto array() const -> std::array<Real32, 4>;
        auto asFloat4fv() const -> const float*;
        auto asFloat4fv() -> float*;

        auto rgb() const -> Color;
        auto brg() const -> Color;
        auto gbr() const -> Color;
        auto bgr() const -> Color;
        auto grb() const -> Color;
        auto rbg() const -> Color;

        Color operator*(const Real32) const;
        bool operator==(const Color &rhs) const;
    };

    extern Color White;
    extern Color Black;
    extern Color Red;
    const Color Nil = Color(-1, -1, -1, -1);

    using rgb = struct {
        Real r;       // a fraction between 0 and 1
        Real g;       // a fraction between 0 and 1
        Real b;       // a fraction between 0 and 1
    };

    using hsv = struct {
        Real h;       // angle in degrees
        Real s;       // a fraction between 0 and 1
        Real v;       // a fraction between 0 and 1
    };

    static auto   rgb2hsv(rgb in) -> hsv;
    static auto   hsv2rgb(hsv in) -> rgb;

    auto rgb2hsv(rgb in) -> hsv
    {
        hsv         out;
        Real      min, max, delta;

        min = in.r < in.g ? in.r : in.g;
        min = min  < in.b ? min  : in.b;

        max = in.r > in.g ? in.r : in.g;
        max = max  > in.b ? max  : in.b;

        out.v = max;                                // v
        delta = max - min;
        if (delta < 0.00001)
        {
            out.s = 0;
            out.h = 0; // undefined, maybe nan?
            return out;
        }
        if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
            out.s = (delta / max);                  // s
        } else {
            // if max is 0, then r = g = b = 0
            // s = 0, h is undefined
            out.s = 0.0;
            out.h = NAN;                            // its now undefined
            return out;
        }
        if( in.r >= max )                           // > is bogus, just keeps compilor happy
            out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
        else
        if( in.g >= max )
            out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
        else
            out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

        out.h *= 60.0;                              // degrees

        if( out.h < 0.0 )
            out.h += 360.0;

        return out;
    }

    auto hsv2rgb(hsv in) -> rgb
    {
        Real      hh, p, q, t, ff;
        long        i;
        rgb         out;

        if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
            out.r = in.v;
            out.g = in.v;
            out.b = in.v;
            return out;
        }
        hh = in.h;
        if(hh >= 360.0) hh = 0.0;
        hh /= 60.0;
        i = (long)hh;
        ff = hh - i;
        p = in.v * (1.0 - in.s);
        q = in.v * (1.0 - (in.s * ff));
        t = in.v * (1.0 - (in.s * (1.0 - ff)));

        switch(i) {
            case 0:
                out.r = in.v;
                out.g = t;
                out.b = p;
                break;
            case 1:
                out.r = q;
                out.g = in.v;
                out.b = p;
                break;
            case 2:
                out.r = p;
                out.g = in.v;
                out.b = t;
                break;

            case 3:
                out.r = p;
                out.g = q;
                out.b = in.v;
                break;
            case 4:
                out.r = t;
                out.g = p;
                out.b = in.v;
                break;
            case 5:
            default:
                out.r = in.v;
                out.g = p;
                out.b = q;
                break;
        }
        return out;
    }
}

#endif