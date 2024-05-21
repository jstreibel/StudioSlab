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
    public:
        typedef std::shared_ptr<Artist> Ptr;

        Artist() = default;

        virtual void draw(const Graph2D &) = 0;

        void setVisibility(bool);
        bool isVisible() const;

        void setAffectGraphRanges(bool);
        bool affectsGraphRanges() const;
    };

    // DefinePointer(Artist)
    typedef ::Slab::Pointer<Artist> Artist_ptr;

}


#endif //V_SHAPE_ARTIST_H
