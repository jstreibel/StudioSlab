//
// Created by joao on 1/10/23.
//

#ifndef STUDIOSLAB_XHAIRARTIST_H
#define STUDIOSLAB_XHAIRARTIST_H

#include "Artist.h"
#include "Math/VectorSpace/Impl/PointSet.h"

namespace Slab::Graphics {

    class XHairArtist : public Artist {
        Math::PointSet XHair;
    public:
        bool draw(const PlottingWindow &window) override;
    };

} // Graphics

#endif //STUDIOSLAB_XHAIRARTIST_H
