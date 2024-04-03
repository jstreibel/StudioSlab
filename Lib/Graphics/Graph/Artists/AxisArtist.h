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
        void computeTicks(const Graph2D &graph);

        void drawXAxis(const Graph2D &graph) const;
        void drawYAxis(const Graph2D &graph) const;

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

        void draw(const Graph2D &) override;

        void setHorizontalUnit(const Unit&);
        auto getHorizontalUnit() const -> const Unit&;
        void setVerticalUnit(const Unit&);

        void setHorizontalAxisLabel(const Str &label);
        auto getVerticalUnit() const -> const Unit&;
        void setVerticalAxisLabel(const Str &label);

        void setHorizontalAxisTicks(Ticks ticks);
    };

} // Math

#endif //STUDIOSLAB_AXISARTIST_H
