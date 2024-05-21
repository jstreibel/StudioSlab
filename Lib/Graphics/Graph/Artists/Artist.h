//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_ARTIST_H
#define V_SHAPE_ARTIST_H

#include "Utils/Utils.h"
#include "Graphics/Types.h"


namespace Graphics {

    class Graph2D;

    class Artist {
        bool visible = true;
        bool affectGraphRanges = false;

        RectR region{-1,1,-1,1};

    public:
        Artist() = default;

        virtual void draw(const Graph2D &) = 0;

        virtual const RectR &getRegion();

        void setVisibility(bool);
        bool isVisible() const;
        virtual bool wantsLegend() const;

        void setAffectGraphRanges(bool);
        bool affectsGraphRanges() const;
    };

    DefinePointer(Artist)
}


#endif //V_SHAPE_ARTIST_H
