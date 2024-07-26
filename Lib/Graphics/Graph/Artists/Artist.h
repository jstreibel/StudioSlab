//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_ARTIST_H
#define V_SHAPE_ARTIST_H

#include "Utils/Utils.h"
#include "Graphics/Types2D.h"


namespace Slab::Graphics {

    class PlottingWindow;

    class Artist {
        Str label = "<unnamed artist>";

        bool visible = true;
        bool affectGraphRanges = false;

    protected:
        RectR region{-1,1,-1,1};

        Str UniqueName(const Str& name) const;

    public:
        Artist() = default;

        virtual bool draw(const PlottingWindow &) = 0;
        virtual bool hasGUI();
        virtual void drawGUI();

        virtual Vector<Pointer<Artist>> getSubArtists();

        virtual const RectR &getRegion();

        virtual void setLabel(Str label);
        virtual auto getLabel() const -> Str;

        virtual auto getXHairInfo(const Point2D &XHairCoord) const -> Str;

        virtual void setVisibility(bool);
        auto isVisible() const -> bool;

        void setAffectGraphRanges(bool);
        bool affectsGraphRanges() const;
    };

    DefinePointers(Artist)
}


#endif //V_SHAPE_ARTIST_H
