//
// Created by joao on 1/10/23.
//

#ifndef STUDIOSLAB_AXISARTIST_H
#define STUDIOSLAB_AXISARTIST_H

#include "Artist.h"
#include "Math/Constants.h"

namespace Graphics {

    class AxisArtist : public Artist {
        void computeTicksSpacings(const Graph2D &graph);

        void drawXAxis(const Graph2D &graph) const;
        void drawYAxis(const Graph2D &graph) const;

        Unit hUnit, vUnit;
        Real xSpacing=1, ySpacing=1;

        Str horizontalAxisLabel = "x";
        Str verticalAxisLabel = "y";
    public:
        AxisArtist();
        explicit AxisArtist(const Unit& horizontal, const Unit& vertical  );

        void draw(const Graph2D &) override;

        void setHorizontalUnit(const Unit&);
        void setVerticalUnit(const Unit&);

        void set_horizontalAxisLabel(const Str &label);
        void set_verticalAxisLabel(const Str &label);
    };

} // Math

#endif //STUDIOSLAB_AXISARTIST_H
