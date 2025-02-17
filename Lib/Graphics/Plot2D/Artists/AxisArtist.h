//
// Created by joao on 1/10/23.
//

#ifndef STUDIOSLAB_AXISARTIST_H
#define STUDIOSLAB_AXISARTIST_H

#include "Artist.h"
#include "Math/Constants.h"

namespace Slab::Graphics {

    using namespace Math;

    class AxisArtist : public Artist {
    public:
        struct Tick {
            Real mark;
            Str label;
        };

        typedef Vector<Tick> Ticks;

    private:
        void computeTicks(const Plot2DWindow &graph);

        void drawXAxis(const Plot2DWindow &graph) const;
        void drawYAxis(const Plot2DWindow &graph) const;

        int y_label_xoffset_in_pixels = 10;
        int y_label_yoffset_in_pixels = 0;

        Unit hUnit, vUnit;
        Real xSpacing=1, ySpacing=1;
        float x_spacing_multiplier = 4.0;
        float y_spacing_multiplier = 2.0;

        Str horizontalAxisLabel = "x";
        Str verticalAxisLabel = "y";

        bool elegant = false;
        bool hTicksManual=false;
        Ticks hTicks;
        bool vTicksManual=false;
        Ticks vTicks;

    public:
        AxisArtist();
        explicit AxisArtist(const Unit& horizontal, const Unit& vertical  );

        bool draw(const Plot2DWindow &) override;

        bool hasGUI() override;

        void drawGUI() override;

        void setHorizontalUnit(const Unit&);
        auto getHorizontalUnit() const -> const Unit&;

        void setHorizontalAxisLabel(const Str &label);
        auto getHorizontalAxisLabel() const -> Str;

        void setVerticalUnit(const Unit&);
        auto getVerticalUnit() const -> const Unit&;

        void setVerticalAxisLabel(const Str &label);
        auto getVerticalAxisLabel() const -> Str;

        void setHorizontalAxisTicks(Ticks ticks);
    };

} // Math

#endif //STUDIOSLAB_AXISARTIST_H
