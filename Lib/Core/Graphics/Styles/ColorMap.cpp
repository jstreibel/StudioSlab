//
// Created by joao on 18/08/23.
//

#include "ColorMap.h"

#include <utility>
#include "Math/Graph/Styles.h"

#define Map(cMap) {cMap.getName(), cMap}

namespace Styles {

    auto blues = ColorMap{
        "blues",
        {
            /* 0.00 */Color::FromBytes(240, 255, 255),
            /* 0.25 */Color::FromBytes(175, 225, 255),
            /* 0.50 */Color::FromBytes(133, 205, 249),
            /* 0.75 */Color::FromBytes(0, 103, 139),
            /* 1.00 */Color::FromBytes(40, 64, 139)
        },
        Color::FromBytes(255,255,255),
        Color::FromBytes(0,0,0)
    };

    auto BrBG = ColorMap("BrBG",{Color{0.0, 0.23529411764705882, 0.18823529411764706},
                                 Color{0.9572472126105345, 0.9599384851980008, 0.9595540176855056},
                                 Color{0.32941176470588235, 0.18823529411764706, 0.0196078431372549}},
                         Color{  0.00000000000000000, 2*0.23529411764705882, 2*0.18823529411764706},
                         Color{2*0.32941176470588235, 2*0.18823529411764706, 2*0.01960784313725490});

    auto rainbow = ColorMap{"rainbow",
                            {Color /* Red:    */ {1.0, 0.0, 0.0},
                             Color /* Orange: */ {1.0, 0.5, 0.0},
                             Color /* Yellow: */ {1.0, 1.0, 0.0},
                             Color /* Green:  */ {0.0, 1.0, 0.0},
                             Color /* Cyan  : */ {.0, 1.0, 1.0},
                             Color /* Blue    */ {.0, .0, 1.0}}};

    std::map<Str, ColorMap> ColorMaps = {Map(blues), Map(BrBG), Map(rainbow)};

    ColorMap::ColorMap(const Str &name, std::vector<Styles::Color> colorSeq, Styles::Color clipped, Styles::Color saturated)
            : name(name), colors(colorSeq), clipped(clipped), saturated(saturated) {
        if(clipped == Styles::Nil) this->clipped = colorSeq[0];
        if(saturated == Styles::Nil) this->saturated = colorSeq.back();
    }

    ColorMap::ColorMap(const Styles::ColorMap &colorMap)
            : ColorMap(colorMap.name, colorMap.colors, colorMap.clipped, colorMap.saturated) {}

    auto ColorMap::mapValue(Real value, Real min, Real max) const -> Styles::Color {
        Real clampedValue = (value - min) / (max-min);

        if (colors.empty()) {
            return {0.0, 0.0, 0.0, 0.0};
        }

        if (clampedValue <= 0.0) {
            return clipped;
        }

        if (clampedValue >= 1.0) {
            return saturated;
        }

        Real interval = 1.0 / static_cast<double>(colors.size() - 1);
        auto index = static_cast<size_t>(clampedValue / interval);
        Real t = (clampedValue - (Real)index * interval) / interval;

        const Color& color1 = colors[index];
        const Color& color2 = colors[index + 1];

        Color result;
        result.r = color1.r + (color2.r - color1.r) * (float)t;
        result.g = color1.g + (color2.g - color1.g) * (float)t;
        result.b = color1.b + (color2.b - color1.b) * (float)t;
        result.a = color1.a + (color2.a - color1.a) * (float)t;

        return result;
    }

    Str ColorMap::getName() const {
        return name;
    }



    auto ColorMap::permute() const -> ColorMap {
        std::vector<Styles::Color> newColors;
        for(auto &c: colors)
            newColors.push_back(c.permute());

        return {name+"_p", newColors, clipped.permute(), saturated.permute()};
    }

    auto ColorMap::bgr() const -> ColorMap {
        std::vector<Styles::Color> newColors;
        for(auto &c: colors)
            newColors.push_back(c.permute(true));

        return {name+"_p", newColors, clipped.permute(true), saturated.permute(true)};
    }

    auto ColorMap::inverse() const -> ColorMap {
        std::vector<Styles::Color> newColors;
        for(auto &c: colors)
            newColors.push_back(c.inverse());

        return {name+"_i", newColors, clipped.inverse(), saturated.inverse()};
    }

    auto ColorMap::reverse() const -> ColorMap {
        std::vector<Styles::Color> newColors;

        for(int i=(int)colors.size()-1; i>=0; --i)
            newColors.push_back(colors[i]);

        return {name+"_r", newColors, saturated, clipped};
    }


};