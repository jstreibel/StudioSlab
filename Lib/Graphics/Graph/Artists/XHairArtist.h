//
// Created by joao on 1/10/23.
//

#ifndef STUDIOSLAB_XHAIRARTIST_H
#define STUDIOSLAB_XHAIRARTIST_H

#include "Artist.h"
#include "Math/Space/Impl/PointSet.h"

namespace Graphics {

    class XHairArtist : public Artist {
        Math::PointSet XHair;
    public:
        void draw(const PlottingWindow &window) override;
    };

} // Graphics

#endif //STUDIOSLAB_XHAIRARTIST_H
