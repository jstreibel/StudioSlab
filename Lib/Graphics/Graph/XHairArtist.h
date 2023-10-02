//
// Created by joao on 1/10/23.
//

#ifndef STUDIOSLAB_XHAIRARTIST_H
#define STUDIOSLAB_XHAIRARTIST_H

#include "Graphics/Artists/Artist.h"
#include "Math/Space/Impl/PointSet.h"

namespace Graphics {

    class XHairArtist : public Artist {
        Spaces::PointSet XHair;
    public:
        void draw(const Graph2D &window) override;
    };

} // Graphics

#endif //STUDIOSLAB_XHAIRARTIST_H
