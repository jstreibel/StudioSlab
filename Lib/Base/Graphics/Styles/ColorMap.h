//
// Created by joao on 18/08/23.
//

#ifndef STUDIOSLAB_COLORMAP_H
#define STUDIOSLAB_COLORMAP_H

#include <vector>
#include <map>

#include "Common/Types.h"
#include "Colors.h"

namespace Styles {

    class ColorMap {
        std::vector<Styles::Color> colors;
        Str name;

    public:
        ColorMap() = default;
        ColorMap(Str name, std::vector<Styles::Color> colorSeq);
        ColorMap(const ColorMap &colorMap);

        auto mapValue(Real clampedValue, Real min=-1, Real max=1) const -> Styles::Color;

        Str getName() const;
    };

    extern std::map<Str, ColorMap> ColorMaps;

}

#endif //STUDIOSLAB_COLORMAP_H
