//
// Created by joao on 24/09/23.
//

#include "Artist.h"
#include "3rdParty/ImGui.h"
#include "Core/Reflection/V2/ReflectionCodecsV2.h"

#include <cctype>
#include <typeinfo>

namespace Slab::Graphics {

    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    namespace {

        auto NormalizeToken(Str raw) -> Str {
            for (char &ch : raw) {
                if (std::isalnum(static_cast<unsigned char>(ch))) {
                    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
                } else {
                    ch = '_';
                }
            }

            Str out;
            out.reserve(raw.size());

            bool lastUnderscore = false;
            for (const char ch : raw) {
                const bool isUnderscore = ch == '_';
                if (isUnderscore && lastUnderscore) continue;
                out.push_back(ch);
                lastUnderscore = isUnderscore;
            }

            while (!out.empty() && out.front() == '_') out.erase(out.begin());
            while (!out.empty() && out.back() == '_') out.pop_back();

            return out;
        }

    } // namespace

    void FArtist::SetVisibility(bool vis) { visible = vis; }

    bool FArtist::IsVisible() const { return visible; }

    void FArtist::SetAffectGraphRanges(bool affects) { affectGraphRanges = affects; }

    bool FArtist::AffectsGraphRanges() const { return affectGraphRanges; }

    const RectR &FArtist::GetRegion() {
        return region;
    }

    void FArtist::SetLabel(Str newLabel) { label = newLabel; }

    auto FArtist::GetLabel() const -> Str { return label; }

    void FArtist::SetArtistId(Str newArtistId) { artistId = std::move(newArtistId); }

    auto FArtist::GetArtistId() const -> const Str & { return artistId; }

    auto FArtist::GetArtistTypeIdV2() const -> Str {
        return typeid(*this).name();
    }

    auto FArtist::DescribeV2Parameters() const -> Vector<FLegacyParameterDescriptorV2> {
        return {
            {
                "artist_id",
                "Artist Id",
                "Stable artist identifier in legacy Plot2D window.",
                ReflectionV2::CTypeIdScalarString,
                false,
                false
            },
            {
                "artist_type",
                "Artist Type",
                "Legacy artist C++ runtime type id.",
                ReflectionV2::CTypeIdScalarString,
                false,
                false
            },
            {
                "label",
                "Label",
                "Artist display label.",
                ReflectionV2::CTypeIdScalarString,
                true,
                false
            },
            {
                "visible",
                "Visible",
                "Whether artist draws in the window.",
                ReflectionV2::CTypeIdScalarBool,
                true,
                false
            },
            {
                "affect_graph_ranges",
                "Affect Graph Ranges",
                "Whether artist contributes to window fit-range review.",
                ReflectionV2::CTypeIdScalarBool,
                true,
                false
            }
        };
    }

    auto FArtist::TryGetV2ParameterValue(const Str &parameterId, Str &encodedValue) const -> bool {
        if (parameterId == "artist_id") {
            if (!artistId.empty()) {
                encodedValue = artistId;
            } else {
                const auto normalizedLabel = NormalizeToken(label);
                encodedValue = normalizedLabel.empty() ? "artist" : normalizedLabel;
            }
            return true;
        }

        if (parameterId == "artist_type") {
            encodedValue = GetArtistTypeIdV2();
            return true;
        }

        if (parameterId == "label") {
            encodedValue = label;
            return true;
        }

        if (parameterId == "visible") {
            encodedValue = visible ? "true" : "false";
            return true;
        }

        if (parameterId == "affect_graph_ranges") {
            encodedValue = affectGraphRanges ? "true" : "false";
            return true;
        }

        return false;
    }

    auto FArtist::TrySetV2ParameterValue(const Str &parameterId,
                                         const Str &encodedValue,
                                         Str *pErrorMessage) -> bool {
        if (parameterId == "label") {
            SetLabel(encodedValue);
            return true;
        }

        if (parameterId == "visible") {
            bool parsed = false;
            if (!ReflectionV2::ParseBoolValue(encodedValue, parsed)) {
                if (pErrorMessage != nullptr) {
                    *pErrorMessage = "Could not parse bool value for parameter 'visible'.";
                }
                return false;
            }

            SetVisibility(parsed);
            return true;
        }

        if (parameterId == "affect_graph_ranges") {
            bool parsed = false;
            if (!ReflectionV2::ParseBoolValue(encodedValue, parsed)) {
                if (pErrorMessage != nullptr) {
                    *pErrorMessage = "Could not parse bool value for parameter 'affect_graph_ranges'.";
                }
                return false;
            }

            SetAffectGraphRanges(parsed);
            return true;
        }

        if (pErrorMessage != nullptr) {
            *pErrorMessage = "Parameter '" + parameterId + "' is not writable by legacy artist bridge.";
        }
        return false;
    }

    auto FArtist::GetXHairInfo(const FPoint2D &XHairCoord) const -> Str {
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
