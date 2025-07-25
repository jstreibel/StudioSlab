//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_ARTIST_H
#define V_SHAPE_ARTIST_H

#include "Utils/Utils.h"
#include "Graphics/Types2D.h"


namespace Slab::Graphics {

    class FPlot2DWindow;

    class FArtist {
        Str label = "<unnamed artist>";

        bool visible = true;
        bool affectGraphRanges = false;

    protected:
        RectR region{-1,1,-1,1};

        Str UniqueName(const Str& name) const;

        DevFloat preferred_w_by_h_ratio = -1;

    public:
        virtual ~FArtist() = default;

        FArtist() = default;

        /**
         * Tell artist to draw its content in argument window.
         * @return true if ok, false if some problem was found.
         */
        virtual bool Draw(const FPlot2DWindow &) = 0;
        virtual bool HasGUI();
        virtual void DrawGUI();

        virtual Vector<TPointer<FArtist>> GetSubArtists();

        virtual const RectR &GetRegion();

        virtual void SetLabel(Str label);
        virtual auto GetLabel() const -> Str;

        virtual auto GetXHairInfo(const Point2D &XHairCoord) const -> Str;

        virtual void SetVisibility(bool);
        auto IsVisible() const -> bool;

        DevFloat GetPreferredRatio() const;

        void SetAffectGraphRanges(bool);
        bool AffectsGraphRanges() const;
    };

    DefinePointers(FArtist)
}


#endif //V_SHAPE_ARTIST_H
