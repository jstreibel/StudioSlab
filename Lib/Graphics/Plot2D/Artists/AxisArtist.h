//
// Created by joao on 1/10/23.
//

#ifndef STUDIOSLAB_AXISARTIST_H
#define STUDIOSLAB_AXISARTIST_H

#include "Artist.h"
#include "Math/Constants.h"

namespace Slab::Graphics {

    using namespace Math;

    class FAxisArtist : public FArtist {
    public:
        struct Tick {
            DevFloat mark;
            Str label;
        };

        typedef Vector<Tick> Ticks;

    private:
        void computeTicks(const FPlot2DWindow &graph);

        void drawXAxis(const FPlot2DWindow &graph) const;
        void drawYAxis(const FPlot2DWindow &graph) const;

        int y_label_xoffset_in_pixels = 10;
        int y_label_yoffset_in_pixels = 0;

        Unit hUnit, vUnit;
        DevFloat xSpacing=1, ySpacing=1;
        float x_spacing_multiplier = 4.0;
        float y_spacing_multiplier = 2.0;

        Str horizontalAxisLabel = "x";
        Str verticalAxisLabel = "y";

        bool elegant = false;
        bool hTicksManual=false;
        Ticks hTicks;
        float hTickHeightMultiplier = 2.0;
        bool vTicksManual=false;
        Ticks vTicks;

    public:
        FAxisArtist();
        explicit FAxisArtist(const Unit& horizontal, const Unit& vertical  );

        bool Draw(const FPlot2DWindow &) override;

        bool HasGUI() override;

        void DrawGUI() override;

        void setHorizontalUnit(const Unit&);
        auto getHorizontalUnit() const -> const Unit&;

        void SetHorizontalAxisLabel(const Str &label);
        auto getHorizontalAxisLabel() const -> Str;

        void setVerticalUnit(const Unit&);
        auto getVerticalUnit() const -> const Unit&;

        void setVerticalAxisLabel(const Str &label);
        auto getVerticalAxisLabel() const -> Str;

        void setHorizontalAxisTicks(Ticks ticks);
    };

} // Math

#endif //STUDIOSLAB_AXISARTIST_H
