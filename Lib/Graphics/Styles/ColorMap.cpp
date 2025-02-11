//
// Created by joao on 18/08/23.
//

#include "ColorMap.h"

#include <utility>
// #include "Graphics/Plot2D/PlotStyle.h"

#define Map(cMap) {cMap->getName(), cMap}

namespace Slab::Graphics {

    auto blues = New<ColorMap>(
        "blues",
        ColorMap::Sequential,
        ColorSequence{
            /* 0.00 */Color::FromBytes(240, 255, 255),
            /* 0.25 */Color::FromBytes(175, 225, 255),
            /* 0.50 */Color::FromBytes(133, 205, 249),
            /* 0.75 */Color::FromBytes(0, 103, 139),
            /* 1.00 */Color::FromBytes(40, 64, 139)
        },
        Nil,
        Color::FromBytes(.25*40, .25*64, .25*139)
    );

    auto afmhot = New<ColorMap>(
            "afmhot",
            ColorMap::Sequential,
            ColorSequence{
                    /* 0.00 */Color(.0,.0,.0),
                    /* 0.25 */Color(.5,.0,.0),
                    /* 0.50 */Color(1.,.5,.0),
                    /* 0.75 */Color(1.,1.,.5),
                    /* 1.00 */Color(1.,1.,1.)
            },
            Nil,
            Color::FromBytes(223, 217, 255)
    );

    auto BrBG = New<ColorMap>(
        "BrBG",
        ColorMap::Divergent,
        ColorSequence{
            Color{0.0, 0.23529411764705882, 0.18823529411764706},
            Color{0.9572472126105345, 0.9599384851980008, 0.9595540176855056},
            Color{0.32941176470588235, 0.18823529411764706, 0.0196078431372549}
        },
        Color{  0.00000000000000000, .5*0.23529411764705882, .5*0.18823529411764706},
        Color{.5*0.32941176470588235, .5*0.18823529411764706, .5*0.01960784313725490});

    auto rainbow = New<ColorMap>(
        "rainbow",
        ColorMap::Cyclic,
        ColorSequence{
            Color /* Red:    */ {1.0, 0.0, 0.0},
            Color /* Orange: */ {1.0, 0.5, 0.0},
            Color /* Yellow: */ {1.0, 1.0, 0.0},
            Color /* Green:  */ {0.0, 1.0, 0.0},
            Color /* Cyan  : */ {.0, 1.0, 1.0},
            Color /* Blue    */ {.0, .0, 1.0},
            Color /* Redagain*/ {1.0, .0, 0.0}}
    );

    #include "colormaps/all_colormaps.inl"

    std::map<Str, Pointer<const ColorMap>> ColorMaps_local = {
            Map(blues),
            Map(BrBG),
            Map(rainbow),
            Map(afmhot)};

    using ColorMapMap = std::map<Str, Pointer<const ColorMap>>;
    ColorMapMap ColorMaps = [](){
        ColorMapMap full_map_of_colormaps(ColorMaps_local.begin(), ColorMaps_local.end());
        full_map_of_colormaps.insert(ColorMaps_auto.begin(), ColorMaps_auto.end());
        return full_map_of_colormaps;
    } ();

    ColorMap::ColorMap(Str name,
                       ColorMapType colorMapType,
                       Vector<Color> colorSeq,
                       Color clipped,
                       Color saturated)
            : name(std::move(name))
            , type(colorMapType)
            , colors(colorSeq)
            , clipped(clipped)
            , saturated(saturated) {

        if(clipped == Nil) this->clipped = colorSeq[0];
        if(saturated == Nil) this->saturated = colorSeq.back();
    }

    ColorMap::ColorMap(const ColorMap &colorMap)
            : ColorMap(colorMap.name, colorMap.type, colorMap.colors, colorMap.clipped, colorMap.saturated) {}

    auto ColorMap::mapValueToColor(Real value) const -> Color {
        if (colors.empty())
            return {0.0, 0.0, 0.0, 0.0};
        if (value <= 0.0)
            return clipped;
        if (value >= 1.0)
            return saturated;

        Real interval = 1.0 / static_cast<double>(colors.size() - 1);
        auto index = static_cast<size_t>(value / interval);
        Real t = (value - (Real)index * interval) / interval;

        const Color& color1 = colors[index];
        const Color& color2 = colors[index + 1];

        Color result;
        result.r = color1.r + (color2.r - color1.r) * (float)t;
        result.g = color1.g + (color2.g - color1.g) * (float)t;
        result.b = color1.b + (color2.b - color1.b) * (float)t;
        result.a = color1.a + (color2.a - color1.a) * (float)t;

        return result;
    }

    auto ColorMap::getName() const -> Str          { return name; }
    auto ColorMap::getType() const -> ColorMapType { return type; }

    auto ColorMap::brg() const -> ColorMap {
        Vector<Color> newColors;
        for(auto &c: colors)
            newColors.push_back(c.permute());

        return {name+"_p", type, newColors, clipped.permute(), saturated.permute()};
    }

    auto ColorMap::bgr() const -> ColorMap {
        Vector<Color> newColors;
        for(auto &c: colors)
            newColors.push_back(c.permute(true));

        return {name+"_p", type, newColors, clipped.permute(true), saturated.permute(true)};
    }

    auto ColorMap::inverse() const -> ColorMap {
        Vector<Color> newColors;
        for(auto &c: colors)
            newColors.push_back(c.inverse());

        return {name+"_i", type, newColors, clipped.inverse(), saturated.inverse()};
    }

    auto ColorMap::reverse() const -> ColorMap {
        Vector<Color> newColors;

        for(int i=(int)colors.size()-1; i>=0; --i)
            newColors.push_back(colors[i]);

        return {name+"_r", type, newColors, saturated, clipped};
    }

    auto ColorMap::clone() const -> Pointer<ColorMap> {
        return New<ColorMap>(*this);
    }

    auto ColorMap::getColorCount() const -> Count {
        return colors.size();
    }

    auto ColorMap::getColor(int i) const -> Color {
        if(i<0) return clipped;
        if(i>(getColorCount()-1)) return saturated;
        return colors[i];
    }

    auto ColorMap::begin() -> ColorSequence::iterator {
        return colors.begin();
    }

    auto ColorMap::end() -> ColorSequence::iterator {
        return colors.end();
    }

    auto ColorMap::begin() const -> ColorSequence::const_iterator {
        return colors.begin();
    }

    auto ColorMap::end() const -> ColorSequence::const_iterator {
        return colors.end();
    }

    Str ColorMap::CategoryToString(ColorMap::ColorMapType category) {
        switch (category) {
            case ColorMap::Sequential:       return "sequential";
            case ColorMap::Divergent:        return "divergent";
            case ColorMap::Cyclic:           return "cyclic";
            case ColorMap::Miscellaneous:    return "miscellaneous";
            case ColorMap::Unknown: default: return "unknown";
        }
    }

};