//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_ARTIST_H
#define V_SHAPE_ARTIST_H

#include "Utils/Utils.h"
#include "Graphics/Types2D.h"


namespace Slab::Graphics {

    class Plot2DWindow;

    class Artist {
        Str label = "<unnamed artist>";

        bool visible = true;
        bool affectGraphRanges = false;

    protected:
        RectR region{-1,1,-1,1};

        Str UniqueName(const Str& name) const;

        Real preferred_w_by_h_ratio = -1;

    public:
        virtual ~Artist() = default;

        Artist() = default;

        /**
         * Tell artist to draw its content in argument window.
         * @return true if ok, false if some problem was found.
         */
        virtual bool draw(const Plot2DWindow &) = 0;
        virtual bool hasGUI();
        virtual void drawGUI();

        virtual Vector<Pointer<Artist>> getSubArtists();

        virtual const RectR &getRegion();

        virtual void setLabel(Str label);
        virtual auto getLabel() const -> Str;

        virtual auto getXHairInfo(const Point2D &XHairCoord) const -> Str;

        virtual void setVisibility(bool);
        auto isVisible() const -> bool;

        Real getPreferredRatio() const;

        void setAffectGraphRanges(bool);
        bool affectsGraphRanges() const;
    };

    DefinePointers(Artist)
}


#endif //V_SHAPE_ARTIST_H
