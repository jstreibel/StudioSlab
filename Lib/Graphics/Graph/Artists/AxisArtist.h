//
// Created by joao on 1/10/23.
//

#ifndef STUDIOSLAB_AXISARTIST_H
#define STUDIOSLAB_AXISARTIST_H

#include "Artist.h"
#include "Math/Constants.h"

namespace Graphics {

    class AxisArtist : public Artist {
    public:
        struct Tick {
            Real mark;
            Str label;
        };

        typedef std::vector<Tick> Ticks;

    private:
        void computeTicks(const PlottingWindow &graph);

        void drawXAxis(const PlottingWindow &graph) const;
        void drawYAxis(const PlottingWindow &graph) const;

        Unit hUnit, vUnit;
        Real xSpacing=1, ySpacing=1;

        Str horizontalAxisLabel = "x";
        Str verticalAxisLabel = "y";

        bool hTicksManual=false;
        Ticks hTicks;
        bool vTicksManual=false;
        Ticks vTicks;

    public:
        AxisArtist();
        explicit AxisArtist(const Unit& horizontal, const Unit& vertical  );

        void draw(const PlottingWindow &) override;

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
