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

namespace Slab::Graphics {

    using ColorSequence = Vector<FColor>;

    class ColorMap final {
    public:
        /* https://matplotlib.org/stable/users/explain/colors/colormaps.html */
        enum ColorMapType {
            Sequential,
            Divergent,
            Cyclic,
            Miscellaneous,
            Unknown
        };

    private:
        Vector<FColor> colors;
        FColor clipped, saturated;
        Str name;
        ColorMapType type = Unknown;

    public:
        ColorMap() = default;
        ColorMap(Str name,
                 ColorMapType,
                 ColorSequence colorSeq,
                 FColor clipped=Nil,
                 FColor saturated=Nil);
        ColorMap(const ColorMap &colorMap);
        virtual ~ColorMap() = default;

        auto clone() const -> Pointer<ColorMap>;

        auto mapValueToColor(DevFloat value) const -> FColor;

        virtual auto
        getColorCount() const -> CountType;

        virtual auto
        getColor(int) const -> FColor;

        auto begin() -> ColorSequence::iterator;
        auto end() -> ColorSequence::iterator;
        auto begin() const -> ColorSequence::const_iterator;
        auto end() const -> ColorSequence::const_iterator;

        auto brg() const -> ColorMap;
        auto bgr() const -> ColorMap;
        auto inverse() const -> ColorMap;
        auto reverse() const -> ColorMap;

        FColor getClippedColor() const;
        FColor getSaturatedColor() const;

        auto getName() const -> Str;
        auto getType() const -> ColorMapType;

        static Str
        CategoryToString(ColorMapType category);
    };

    extern std::map<Str, Pointer<const ColorMap>> ColorMaps;

}

#endif //STUDIOSLAB_COLORMAP_H
