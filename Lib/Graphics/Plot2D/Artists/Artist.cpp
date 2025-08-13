//
// Created by joao on 24/09/23.
//

#include "Artist.h"
#include "3rdParty/ImGui.h"

namespace Slab::Graphics {

    void FArtist::SetVisibility(bool vis) { visible = vis; }

    bool FArtist::IsVisible() const { return visible; }

    void FArtist::SetAffectGraphRanges(bool affects) { affectGraphRanges = affects; }

    bool FArtist::AffectsGraphRanges() const { return affectGraphRanges; }

    const RectR &FArtist::GetRegion() {
        return region;
    }

    void FArtist::SetLabel(Str newLabel) { label = newLabel; }

    auto FArtist::GetLabel() const -> Str { return label; }

    auto FArtist::GetXHairInfo(const Point2D &XHairCoord) const -> Str {
        return Str();
    }

    void FArtist::DrawGUI() {
        // ImGui::InputText(UniqueName("Label"), )
    }

    bool FArtist::HasGUI() {
        return false;
    }

    Str FArtist::UniqueName(const Str& name) const { return name + "##" + label; }

    Vector<TPointer<FArtist>> FArtist::GetSubArtists() {
        return {};
    }

    DevFloat FArtist::GetPreferredRatio() const {
        return preferred_w_by_h_ratio;
    }


}