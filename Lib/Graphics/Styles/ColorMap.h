//
// Created by joao on 18/08/23.
//

#ifndef STUDIOSLAB_COLORMAP_H
#define STUDIOSLAB_COLORMAP_H

#include <vector>
#include <map>
#include <functional>

#include "Utils/Types.h"
#include "Colors.h"
#include "Graphics/Graph/PlotStyle.h"

namespace Graphics {

    class ColorMap {
    public:
        /* https://matplotlib.org/stable/users/explain/colors/colormaps.html */
        enum ColorMapType {
            Sequential,
            Divergent,
            Cyclic,
        };

    private:
        std::vector<Color> colors;
        Color clipped, saturated;
        Str name;
        ColorMapType type;

    public:
        ColorMap() = default;
        ColorMap(Str name, ColorMapType, std::vector<Color> colorSeq,
                 Color clipped=Nil,
                 Color saturated=Nil);
        ColorMap(const ColorMap &colorMap);

        auto mapValueToColor(Real value) const -> Color;

        auto permute() const -> ColorMap;
        auto bgr() const -> ColorMap;
        auto inverse() const -> ColorMap;
        auto reverse() const -> ColorMap;

        Color getClippedColor() const;
        Color getSaturatedColor() const;

        auto getName() const -> Str;
        auto getType() const -> ColorMapType;
    };

    extern std::map<Str, ColorMap> ColorMaps;

}

#endif //STUDIOSLAB_COLORMAP_H
