#include <catch2/catch_all.hpp>

#include "Core/Ontology/V2/OntologyGraphV2.h"
#include "Graphics/Plot2D/V2/Plot2DV2.h"

#include <filesystem>
#include <limits>
#include <map>

namespace {

    using namespace Slab;
    using namespace Slab::Core::Ontology::V2;
    using namespace Slab::Graphics::Plot2D::V2;

    [[nodiscard]] auto FindOntologyResourcesDirectoryV2() -> std::filesystem::path {
        const auto suffix = std::filesystem::path("Resources") / "Ontologies";
        auto cursor = std::filesystem::current_path();

        while (true) {
            const auto candidate = cursor / suffix;
            if (std::filesystem::exists(candidate / "studioslab-ontology.schema.json") &&
                std::filesystem::exists(candidate / "global-descent.ontology.json")) {
                return candidate;
            }

            const auto parent = cursor.parent_path();
            if (parent.empty() || parent == cursor) break;
            cursor = parent;
        }

        return {};
    }

    [[nodiscard]] auto FindProjectedNodeById(const FOntologyGraphProjection &projection, const Str &nodeId)
        -> const FOntologyProjectedNodeV2 * {
        return projection.FindNode(nodeId);
    }

    [[nodiscard]] auto CountRectangleCommands(const FPlotDrawListV2 &drawList) -> int {
        int count = 0;
        for (const auto &command : drawList.GetCommands()) {
            if (std::holds_alternative<FRectangleCommandV2>(command)) {
                ++count;
            }
        }
        return count;
    }

    [[nodiscard]] auto CountTextCommands(const FPlotDrawListV2 &drawList) -> int {
        int count = 0;
        for (const auto &command : drawList.GetCommands()) {
            if (std::holds_alternative<FTextCommandV2>(command)) {
                ++count;
            }
        }
        return count;
    }

    [[nodiscard]] auto HasTextContaining(const FPlotDrawListV2 &drawList, const Str &needle) -> bool {
        for (const auto &command : drawList.GetCommands()) {
            const auto *text = std::get_if<FTextCommandV2>(&command);
            if (text == nullptr) continue;
            if (text->Text.find(needle) != Str::npos) return true;
        }
        return false;
    }

    [[nodiscard]] auto FindTextCommandContaining(const FPlotDrawListV2 &drawList,
                                                 const Str &needle,
                                                 const EPlotCoordinateSpaceV2 coordinateSpace = EPlotCoordinateSpaceV2::Plot)
        -> const FTextCommandV2 * {
        for (const auto &command : drawList.GetCommands()) {
            const auto *text = std::get_if<FTextCommandV2>(&command);
            if (text == nullptr) continue;
            if (text->CoordinateSpace != coordinateSpace) continue;
            if (text->Text.find(needle) == Str::npos) continue;
            return text;
        }

        return nullptr;
    }

} // namespace

TEST_CASE("Ontology graph bundle loads schema global and study documents", "[OntologyGraphV2]") {
    const auto ontologyDirectory = FindOntologyResourcesDirectoryV2();
    REQUIRE_FALSE(ontologyDirectory.empty());

    const auto schemaPath = ontologyDirectory / "studioslab-ontology.schema.json";
    const auto globalPath = ontologyDirectory / "global-descent.ontology.json";
    const auto studyPaths = DiscoverOntologyStudyDocumentsV2(ontologyDirectory);

    REQUIRE(studyPaths.size() >= 2);

    const auto bundle = LoadOntologyGraphBundleV2(schemaPath, globalPath, studyPaths);

    REQUIRE(bundle.Schema.bLoaded);
    REQUIRE(bundle.GlobalDocument.bLoaded);
    REQUIRE(bundle.StudyDocuments.size() >= 2);
    REQUIRE(bundle.Diagnostics.ErrorCount() == 0);

    REQUIRE(bundle.FindNode("study:ho") != nullptr);
    REQUIRE(bundle.FindNode("study:kg") != nullptr);
    REQUIRE(bundle.FindNode("descent:FirstOrderExplicitODE") != nullptr);
    REQUIRE(bundle.FindNode("solver:RK4") != nullptr);
    REQUIRE(bundle.FindNode("realization:RegularGrid") != nullptr);
    REQUIRE(bundle.FindNode("requirement:BoundaryConditions") != nullptr);
}

TEST_CASE("Ontology projection preserves deterministic reachable and blocked regions", "[OntologyGraphV2]") {
    const auto ontologyDirectory = FindOntologyResourcesDirectoryV2();
    REQUIRE_FALSE(ontologyDirectory.empty());

    const auto bundle = LoadOntologyGraphBundleV2(
        ontologyDirectory / "studioslab-ontology.schema.json",
        ontologyDirectory / "global-descent.ontology.json",
        DiscoverOntologyStudyDocumentsV2(ontologyDirectory));
    REQUIRE(bundle.Diagnostics.ErrorCount() == 0);

    const FOntologyGraphFilterStateV2 defaultFilters;
    const auto hoProjectionA = BuildOntologyGraphProjectionV2(bundle, "study:ho", defaultFilters);
    const auto hoProjectionB = BuildOntologyGraphProjectionV2(bundle, "study:ho", defaultFilters);

    const auto *firstOrderA = FindProjectedNodeById(hoProjectionA, "descent:FirstOrderExplicitODE");
    const auto *rk4A = FindProjectedNodeById(hoProjectionA, "solver:RK4");
    const auto *trajectoryA = FindProjectedNodeById(hoProjectionA, "artifact:TrajectorySeries");
    REQUIRE(firstOrderA != nullptr);
    REQUIRE(rk4A != nullptr);
    REQUIRE(trajectoryA != nullptr);
    REQUIRE(firstOrderA->ActivationStatus == EOntologyActivationStatusV2::Reachable);
    REQUIRE(rk4A->ActivationStatus == EOntologyActivationStatusV2::Reachable);
    REQUIRE(trajectoryA->ActivationStatus == EOntologyActivationStatusV2::Reachable);

    const auto *firstOrderB = FindProjectedNodeById(hoProjectionB, "descent:FirstOrderExplicitODE");
    const auto *rk4B = FindProjectedNodeById(hoProjectionB, "solver:RK4");
    const auto *trajectoryB = FindProjectedNodeById(hoProjectionB, "artifact:TrajectorySeries");
    REQUIRE(firstOrderB != nullptr);
    REQUIRE(rk4B != nullptr);
    REQUIRE(trajectoryB != nullptr);
    REQUIRE(firstOrderA->Position.x == Catch::Approx(firstOrderB->Position.x));
    REQUIRE(firstOrderA->Position.y == Catch::Approx(firstOrderB->Position.y));
    REQUIRE(rk4A->Position.x == Catch::Approx(rk4B->Position.x));
    REQUIRE(rk4A->Position.y == Catch::Approx(rk4B->Position.y));
    REQUIRE(trajectoryA->Position.x == Catch::Approx(trajectoryB->Position.x));
    REQUIRE(trajectoryA->Position.y == Catch::Approx(trajectoryB->Position.y));

    FOntologyGraphFilterStateV2 blockedFilters;
    blockedFilters.bShowBlockedRequirementsOnly = true;
    const auto kgBlockedProjection = BuildOntologyGraphProjectionV2(bundle, "study:kg", blockedFilters);

    const auto *boundaryNode = FindProjectedNodeById(kgBlockedProjection, "requirement:BoundaryConditions");
    const auto *metricNode = FindProjectedNodeById(kgBlockedProjection, "requirement:MetricOrCoordinateSpecialization");
    REQUIRE(boundaryNode != nullptr);
    REQUIRE(metricNode != nullptr);
    REQUIRE(boundaryNode->ActivationStatus == EOntologyActivationStatusV2::Blocked);
    REQUIRE(metricNode->ActivationStatus == EOntologyActivationStatusV2::Blocked);
    REQUIRE(FindProjectedNodeById(kgBlockedProjection, "realization:RegularGrid") == nullptr);

    FOntologyGraphFilterStateV2 pathFilters;
    pathFilters.bShowRealizationRecipeArtifactPathOnly = true;
    const auto kgPathProjection = BuildOntologyGraphProjectionV2(bundle, "study:kg", pathFilters);
    const auto *gridNode = FindProjectedNodeById(kgPathProjection, "realization:RegularGrid");
    const auto *fdNode = FindProjectedNodeById(kgPathProjection, "realization:FiniteDifferenceDiscretization");
    const auto *artifactNode = FindProjectedNodeById(kgPathProjection, "artifact:FieldSnapshotSeries");
    REQUIRE(gridNode != nullptr);
    REQUIRE(fdNode != nullptr);
    REQUIRE(artifactNode != nullptr);
    REQUIRE(gridNode->ActivationStatus == EOntologyActivationStatusV2::Reachable);
    REQUIRE(fdNode->ActivationStatus == EOntologyActivationStatusV2::Reachable);
    REQUIRE(artifactNode->ActivationStatus == EOntologyActivationStatusV2::Reachable);
}

TEST_CASE("Ontology projection keeps dense study layout breathable", "[OntologyGraphV2]") {
    const auto ontologyDirectory = FindOntologyResourcesDirectoryV2();
    REQUIRE_FALSE(ontologyDirectory.empty());

    const auto bundle = LoadOntologyGraphBundleV2(
        ontologyDirectory / "studioslab-ontology.schema.json",
        ontologyDirectory / "global-descent.ontology.json",
        DiscoverOntologyStudyDocumentsV2(ontologyDirectory));
    REQUIRE(bundle.Diagnostics.ErrorCount() == 0);

    const auto projection = BuildOntologyGraphProjectionV2(bundle, "study:ho", {});
    REQUIRE(projection.Nodes.size() > 12);

    std::map<DevFloat, Vector<const FOntologyProjectedNodeV2 *>> nodesByColumnX;
    for (const auto &node : projection.Nodes) {
        nodesByColumnX[node.Position.x].push_back(&node);
    }

    Vector<DevFloat> columnXs;
    columnXs.reserve(nodesByColumnX.size());
    for (const auto &[columnX, nodes] : nodesByColumnX) {
        (void) nodes;
        columnXs.push_back(columnX);
    }
    std::sort(columnXs.begin(), columnXs.end());
    REQUIRE(columnXs.size() >= 5);

    auto minColumnGap = std::numeric_limits<DevFloat>::max();
    for (std::size_t index = 1; index < columnXs.size(); ++index) {
        minColumnGap = std::min(minColumnGap, columnXs[index] - columnXs[index - 1]);
    }

    const auto totalSpan = columnXs.back() - columnXs.front();
    INFO("Minimum ontology column gap: " << minColumnGap);
    INFO("Total ontology column span: " << totalSpan);
    REQUIRE(minColumnGap > 8.5);
    REQUIRE(totalSpan < 160.0);
}

TEST_CASE("Ontology graph artist renders and supports click selection", "[OntologyGraphV2][Plot2DV2]") {
    const auto ontologyDirectory = FindOntologyResourcesDirectoryV2();
    REQUIRE_FALSE(ontologyDirectory.empty());

    const auto bundle = LoadOntologyGraphBundleV2(
        ontologyDirectory / "studioslab-ontology.schema.json",
        ontologyDirectory / "global-descent.ontology.json",
        DiscoverOntologyStudyDocumentsV2(ontologyDirectory));
    REQUIRE(bundle.Diagnostics.ErrorCount() == 0);

    FOntologyGraphFilterStateV2 filters;
    filters.bFocusActiveReachableRegion = true;
    const auto projection = BuildOntologyGraphProjectionV2(bundle, "study:ho", filters);

    auto artist = New<FOntologyGraphArtistV2>();
    artist->SetProjection(projection);

    FPlot2DWindowV2 window("Ontology Graph Test", {-12.0, 96.0, -38.0, 12.0}, {0, 900, 0, 640});
    window.AddArtist(artist, 0);

    FRecordingRenderBackendV2 backend;
    REQUIRE(window.Render(backend));

    const auto &drawList = backend.GetLastDrawList();
    REQUIRE(CountRectangleCommands(drawList) > 0);
    REQUIRE(CountTextCommands(drawList) > 0);
    REQUIRE(HasTextContaining(drawList, "Harmonic oscillator"));
    REQUIRE(HasTextContaining(drawList, "Runge-Kutta 4"));
    REQUIRE(HasTextContaining(drawList, "solver"));
    REQUIRE_FALSE(HasTextContaining(drawList, "SolverClass"));
    REQUIRE_FALSE(HasTextContaining(drawList, "DescentClass"));

    const auto *studyRoot = projection.FindNode("study:ho");
    REQUIRE(studyRoot != nullptr);

    const FPlotPointerEventV2 pressEvent{
        .Kind = EPlotPointerEventKindV2::Button,
        .PlotPosition = studyRoot->Position,
        .ViewportPosition = studyRoot->Position,
        .Button = Slab::Graphics::MouseButton_LEFT,
        .ButtonState = Slab::Graphics::Press
    };
    const FPlotPointerEventV2 releaseEvent{
        .Kind = EPlotPointerEventKindV2::Button,
        .PlotPosition = studyRoot->Position,
        .ViewportPosition = studyRoot->Position,
        .Button = Slab::Graphics::MouseButton_LEFT,
        .ButtonState = Slab::Graphics::Release
    };

    const auto frame = window.BuildFrameContext();
    REQUIRE(artist->HandlePointerEvent(frame, pressEvent));
    REQUIRE(artist->HandlePointerEvent(frame, releaseEvent));
    REQUIRE(artist->GetSelection().Kind == EOntologyElementKindV2::Node);
    REQUIRE(artist->GetSelection().ElementId == "study:ho");
}

TEST_CASE("Ontology graph plot text scales with zoom", "[OntologyGraphV2][Plot2DV2]") {
    const auto ontologyDirectory = FindOntologyResourcesDirectoryV2();
    REQUIRE_FALSE(ontologyDirectory.empty());

    const auto bundle = LoadOntologyGraphBundleV2(
        ontologyDirectory / "studioslab-ontology.schema.json",
        ontologyDirectory / "global-descent.ontology.json",
        DiscoverOntologyStudyDocumentsV2(ontologyDirectory));
    REQUIRE(bundle.Diagnostics.ErrorCount() == 0);

    const auto projection = BuildOntologyGraphProjectionV2(bundle, "study:ho", {});

    auto artist = New<FOntologyGraphArtistV2>();
    artist->SetProjection(projection);

    FPlot2DWindowV2 window("Ontology Graph Zoom Test", {-12.0, 96.0, -38.0, 12.0}, {0, 900, 0, 640});
    window.AddArtist(artist, 0);
    REQUIRE(window.FitRegionToArtists(0.08));

    FRecordingRenderBackendV2 backend;
    REQUIRE(window.Render(backend));

    const auto *fittedTitle = FindTextCommandContaining(backend.GetLastDrawList(), "Harmonic oscillator");
    REQUIRE(fittedTitle != nullptr);
    REQUIRE(fittedTitle->FontScale == Catch::Approx(0.88).margin(0.08));
    const auto fittedFontScale = fittedTitle->FontScale;

    const auto fittedRegion = window.GetRegion();
    const auto zoomedRegion = Slab::Graphics::RectR{
        fittedRegion.xCenter() - (0.30 * fittedRegion.GetWidth()),
        fittedRegion.xCenter() + (0.30 * fittedRegion.GetWidth()),
        fittedRegion.yCenter() - (0.30 * fittedRegion.GetHeight()),
        fittedRegion.yCenter() + (0.30 * fittedRegion.GetHeight())
    };
    window.SetRegion(zoomedRegion);

    REQUIRE(window.Render(backend));

    const auto *zoomedTitle = FindTextCommandContaining(backend.GetLastDrawList(), "Harmonic oscillator");
    REQUIRE(zoomedTitle != nullptr);
    REQUIRE(zoomedTitle->FontScale > fittedFontScale);
    REQUIRE(zoomedTitle->FontScale >= Catch::Approx(1.38).margin(0.05));
}
