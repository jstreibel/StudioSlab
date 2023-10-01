//
// Created by joao on 1/10/23.
//

#ifndef STUDIOSLAB_AXISARTIST_H
#define STUDIOSLAB_AXISARTIST_H

#include "Core/Graphics/Artists/Artist.h"
#include "Math/Constants.h"

namespace Math {

    class AxisArtist : public Artist {
        void computeTicksSpacings();

        void drawXAxis(const RectI &viewport);
        void drawYAxis(const RectI &viewport);

        const RectR &tiedRegion;
        Unit hUnit, vUnit;
        Real xSpacing=1, ySpacing=1;
    public:
        explicit AxisArtist(const RectR &regionToTie,
                            const Unit& horizontal =Constants::One,
                            const Unit& vertical   =Constants::One);

        void draw(const RectI &viewport) override;

        void tieToRegion(RectR);
    };

} // Math

#endif //STUDIOSLAB_AXISARTIST_H