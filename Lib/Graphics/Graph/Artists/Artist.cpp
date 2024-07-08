//
// Created by joao on 24/09/23.
//

#include "Artist.h"
#include "3rdParty/ImGui.h"

namespace Slab::Graphics {

    void Artist::setVisibility(bool vis) { visible = vis; }

    bool Artist::isVisible() const { return visible; }

    void Artist::setAffectGraphRanges(bool affects) { affectGraphRanges = affects; }

    bool Artist::affectsGraphRanges() const { return affectGraphRanges; }

    const RectR &Artist::getRegion() {
        return region;
    }

    void Artist::setLabel(Str newLabel) { label = newLabel; }

    auto Artist::getLabel() const -> Str { return label; }

    auto Artist::getXHairInfo(const Point2D &XHairCoord) const -> Str {
        return Str();
    }

    void Artist::drawGUI() {
        // ImGui::InputText(UniqueName("Label"), )
    }

    bool Artist::hasGUI() {
        return false;
    }

    Str Artist::UniqueName(const Str& name) const { return name + "##" + label; }

}