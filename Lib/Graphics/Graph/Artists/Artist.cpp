//
// Created by joao on 24/09/23.
//

#include "Artist.h"

namespace Graphics {

    void Artist::setVisibility(bool vis) { visible = vis; }
    bool Artist::isVisible() const { return visible; }

    void Artist::setAffectGraphRanges(bool affects) { affectGraphRanges=affects; }
    bool Artist::affectsGraphRanges() const { return affectGraphRanges; }

    const RectR &Artist::getRegion() {
        return region;
    }

    bool Artist::wantsLegend() const {
        return false;
    }

}