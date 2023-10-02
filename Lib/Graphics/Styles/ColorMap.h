//
// Created by joao on 18/08/23.
//

#ifndef STUDIOSLAB_COLORMAP_H
#define STUDIOSLAB_COLORMAP_H

#include <vector>
#include <map>

#include "Utils/Types.h"
#include "Colors.h"
#include "Graphics/Graph/Styles.h"

namespace Styles {

    class ColorMap {
        std::vector<Styles::Color> colors;
        Styles::Color clipped, saturated;
        Str name;

    public:
        ColorMap() = default;
        ColorMap(const Str &name, std::vector<Styles::Color> colorSeq, Styles::Color clipped=Styles::Nil, Styles::Color saturated=Styles::Nil);
        ColorMap(const ColorMap &colorMap);

        auto mapValue(Real clampedValue, Real min=-1, Real max=1) const -> Styles::Color;

        auto permute() const -> ColorMap;
        auto bgr() const -> ColorMap;
        auto inverse() const -> ColorMap;
        auto reverse() const -> ColorMap;

        Color getClippedColor() const;
        Color getSaturatedColor() const;

        Str getName() const;
    };

    extern std::map<Str, ColorMap> ColorMaps;

}

#endif //STUDIOSLAB_COLORMAP_H
