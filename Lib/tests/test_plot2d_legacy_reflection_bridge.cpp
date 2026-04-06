#include <catch2/catch_all.hpp>

#include <algorithm>

#include "Graphics/Plot2D/Artists/Artist.h"

namespace {

    using namespace Slab;
    using namespace Slab::Graphics;

    class FNoopLegacyArtist final : public FArtist {
    public:
        auto Draw(const FPlot2DWindow &) -> bool override {
            return true;
        }
    };

    auto HasParameterId(const Vector<FArtist::FLegacyParameterDescriptorV2> &parameters, const Str &parameterId) -> bool {
        return std::any_of(parameters.begin(), parameters.end(), [&](const auto &parameter) {
            return parameter.ParameterId == parameterId;
        });
    }

} // namespace

TEST_CASE("Legacy artist reflection hooks expose base parameters", "[Plot2D][Legacy][ReflectionV2]") {
    FNoopLegacyArtist artist;
    artist.SetLabel("demo artist");

    const auto parameters = artist.DescribeV2Parameters();
    REQUIRE(HasParameterId(parameters, "artist_id"));
    REQUIRE(HasParameterId(parameters, "artist_type"));
    REQUIRE(HasParameterId(parameters, "label"));
    REQUIRE(HasParameterId(parameters, "visible"));
    REQUIRE(HasParameterId(parameters, "affect_graph_ranges"));

    Str value;
    REQUIRE(artist.TryGetV2ParameterValue("artist_id", value));
    REQUIRE_FALSE(value.empty());

    REQUIRE(artist.TrySetV2ParameterValue("label", "renamed artist"));
    REQUIRE(artist.GetLabel() == "renamed artist");

    REQUIRE(artist.TrySetV2ParameterValue("visible", "false"));
    REQUIRE_FALSE(artist.IsVisible());

    REQUIRE(artist.TrySetV2ParameterValue("affect_graph_ranges", "true"));
    REQUIRE(artist.AffectsGraphRanges());

    Str errorMessage;
    REQUIRE_FALSE(artist.TrySetV2ParameterValue("visible", "not_bool", &errorMessage));
    REQUIRE_FALSE(errorMessage.empty());
}
