#include <catch2/catch_all.hpp>

#include "Core/Reflection/V2/ReflectionSemanticsOverlayV1.h"
#include "Graphics/Plot2D/V2/Plot2DV2.h"
#include "Graphics/Plot2D/V2/PlotReflectionSchemaV2.h"

#include <algorithm>
#include <cmath>

namespace {

    class FCosXFunction final : public Slab::Math::RtoR::Function {
    public:
        auto operator()(const Slab::DevFloat x) const -> Slab::DevFloat override {
            return std::cos(x);
        }

        auto Clone() const -> Slab::TPointer<Type> override {
            return Slab::New<FCosXFunction>(*this);
        }
    };

} // namespace

TEST_CASE("Semantic V1 catalog supports registration and signature matching", "[ReflectionV2][SemanticsV1]") {
    using namespace Slab::Core::Reflection::V2;

    FSemanticCatalogV1 semanticCatalog;
    RegisterSemanticSeedLibraryV1(semanticCatalog);

    FSpaceSchemaV1 customSpace;
    customSpace.SpaceId = "test.space.scalar_like";
    customSpace.DisplayName = "ScalarLike";
    customSpace.CarrierTypeId = CTypeIdScalarFloat64;
    customSpace.Tags = {"test", "scalar"};
    REQUIRE(semanticCatalog.RegisterSpace(customSpace));
    REQUIRE(semanticCatalog.FindSpaceById(customSpace.SpaceId) != nullptr);

    FSemanticOperatorSchemaV1 scaleOperator;
    scaleOperator.OperatorId = "test.operator.scale";
    scaleOperator.DisplayName = "Scale";
    scaleOperator.Kind = ESemanticOperatorKindV1::Transform;
    scaleOperator.DeclaredProperties = SemanticPropertyMaskV1(ESemanticOperatorPropertyV1::Linear);
    scaleOperator.DomainPorts = {MakeSemanticPortSchemaV1("x", "x", CSpaceIdScalarRealV1)};
    scaleOperator.CodomainPorts = {MakeSemanticPortSchemaV1("y", "y", CSpaceIdScalarRealV1)};
    REQUIRE(semanticCatalog.RegisterOperator(scaleOperator));

    FImplementationBindingV1 binding;
    binding.SemanticOperatorId = scaleOperator.OperatorId;
    binding.TargetInterfaceId = "test.interface";
    binding.TargetOperationId = "command.test.scale";
    binding.CostHints.ComplexityClass = "O(1)";
    REQUIRE(semanticCatalog.RegisterBinding(binding));

    const auto bindings = semanticCatalog.QueryBindingsForOperator(scaleOperator.OperatorId);
    REQUIRE(bindings.size() == 1);
    CHECK(bindings.front().TargetInterfaceId == "test.interface");
    CHECK(bindings.front().TargetOperationId == "command.test.scale");

    const auto directMatches = semanticCatalog.QueryOperatorsForSignature(
        {CSpaceIdScalarRealV1},
        {CSpaceIdScalarRealV1});
    CHECK(std::any_of(directMatches.begin(), directMatches.end(), [&](const auto *schema) {
        return schema != nullptr && schema->OperatorId == "test.operator.scale";
    }));

    const auto strictNoCoercionMatches = semanticCatalog.QueryOperatorsForSignature(
        {CSpaceIdScalarIntV1},
        {CSpaceIdScalarRealV1});
    CHECK_FALSE(std::any_of(strictNoCoercionMatches.begin(), strictNoCoercionMatches.end(), [&](const auto *schema) {
        return schema != nullptr && schema->OperatorId == "test.operator.scale";
    }));

    const auto matchResults = semanticCatalog.QueryOperatorMatchesForSignature(
        {CSpaceIdScalarIntV1},
        {CSpaceIdScalarRealV1});
    const auto matchIt = std::find_if(matchResults.begin(), matchResults.end(), [](const auto &result) {
        return result.OperatorId == "test.operator.scale";
    });
    REQUIRE(matchIt != matchResults.end());
    CHECK(matchIt->Reason == ESignatureMatchReasonV1::CompatibleButNeedsCoercion);
    CHECK(std::any_of(
        matchIt->SuggestedCoercionOperatorIds.begin(),
        matchIt->SuggestedCoercionOperatorIds.end(),
        [](const auto &operatorId) { return operatorId == COperatorIdCoerceIntToRealV1; }));

    const auto coercionPath = semanticCatalog.QuerySuggestedCoercions(CSpaceIdScalarIntV1, CSpaceIdScalarRealV1);
    REQUIRE_FALSE(coercionPath.empty());
    CHECK(coercionPath.front() == CSpaceIdScalarIntV1);
    CHECK(coercionPath.back() == CSpaceIdScalarRealV1);
}

TEST_CASE("Semantic overlay attaches to reflection catalog entities", "[ReflectionV2][SemanticsV1]") {
    using namespace Slab::Core::Reflection::V2;

    FReflectionCatalogV2 reflectionCatalog;
    FInterfaceSchemaV2 interfaceSchema;
    interfaceSchema.InterfaceId = "test.interface";
    interfaceSchema.DisplayName = "Test Interface";

    FParameterSchemaV2 parameter;
    parameter.ParameterId = "alpha";
    parameter.TypeId = CTypeIdScalarFloat64;
    interfaceSchema.Parameters.push_back(parameter);

    FOperationSchemaV2 operation;
    operation.OperationId = "command.test.scale";
    operation.Kind = EOperationKind::Command;
    operation.Inputs = {
        FOperationFieldSchemaV2{"value", "Value", "Input value", CTypeIdScalarFloat64, true}
    };
    operation.Outputs = {
        FOperationFieldSchemaV2{"result", "Result", "Output value", CTypeIdScalarFloat64, true}
    };
    interfaceSchema.Operations.push_back(operation);
    reflectionCatalog.Interfaces.push_back(interfaceSchema);

    FSemanticCatalogV1 semanticCatalog;
    RegisterSemanticSeedLibraryV1(semanticCatalog);

    FReflectionSemanticsOverlayV1 overlay;
    AttachSemanticsForCatalogV1(reflectionCatalog, overlay, semanticCatalog);

    const auto parameterSpace = overlay.FindParameterSpace("test.interface", "alpha");
    REQUIRE(parameterSpace.has_value());
    CHECK(*parameterSpace == CSpaceIdScalarRealV1);

    const auto inputSpace = overlay.FindOperationInputPortSpace("test.interface", "command.test.scale", "value");
    REQUIRE(inputSpace.has_value());
    CHECK(*inputSpace == CSpaceIdScalarRealV1);

    const auto outputSpace = overlay.FindOperationOutputPortSpace("test.interface", "command.test.scale", "result");
    REQUIRE(outputSpace.has_value());
    CHECK(*outputSpace == CSpaceIdScalarRealV1);

    REQUIRE(overlay.AttachOperationAsImplementationOf(
        "test.interface",
        "command.test.scale",
        COperatorIdIdentityV1,
        {},
        {},
        &semanticCatalog));

    const auto semanticOperatorId = overlay.FindOperationSemanticOperatorId("test.interface", "command.test.scale");
    REQUIRE(semanticOperatorId.has_value());
    CHECK(*semanticOperatorId == COperatorIdIdentityV1);

    const auto identityBindings = semanticCatalog.QueryBindingsForOperator(COperatorIdIdentityV1);
    CHECK(std::any_of(identityBindings.begin(), identityBindings.end(), [](const auto &binding) {
        return binding.TargetInterfaceId == "test.interface" &&
               binding.TargetOperationId == "command.test.scale";
    }));
}

TEST_CASE("Semantic overlay binds to real plot reflection operations", "[ReflectionV2][SemanticsV1][Plot2DV2]") {
    using namespace Slab;
    using namespace Slab::Core::Reflection::V2;
    using namespace Slab::Graphics::Plot2D::V2;

    FPlot2DWindowV2 window("Semantics Plot Binding", {-1.0, 1.0, -1.0, 1.0});
    auto functionArtist = New<FRtoRFunctionArtistV2>(
        New<FCosXFunction>(),
        Slab::Graphics::PlotStyle(
            Slab::Graphics::FlatBlue,
            Slab::Graphics::LineStrip,
            false,
            Slab::Graphics::Nil,
            1.4f),
        32,
        -1.5,
        1.5);
    window.AddArtist(functionArtist);

    FPlotReflectionCatalogV2 plotCatalog;
    plotCatalog.RefreshFromLiveWindows();

    FSemanticCatalogV1 semanticCatalog;
    RegisterSemanticSeedLibraryV1(semanticCatalog);

    FReflectionSemanticsOverlayV1 overlay;
    AttachSemanticsForCatalogV1(plotCatalog.GetCatalog(), overlay, semanticCatalog);

    const auto functionInterfaceId = Str("v2.plot.artist.") + window.GetWindowId() + "." + functionArtist->GetArtistId();

    const auto functionInputSpace = overlay.FindOperationInputPortSpace(
        functionInterfaceId,
        CPlotOperationIdCommandArtistSetFunctionV2,
        "value");
    REQUIRE(functionInputSpace.has_value());
    CHECK(*functionInputSpace == CSpaceIdFunctionRtoRV1);

    const auto setFunctionSemanticOpDefault = overlay.FindOperationSemanticOperatorId(
        functionInterfaceId,
        CPlotOperationIdCommandArtistSetFunctionV2);
    CHECK_FALSE(setFunctionSemanticOpDefault.has_value());

    const auto identityBindings = semanticCatalog.QueryBindingsForOperator(COperatorIdIdentityV1);
    CHECK_FALSE(std::any_of(identityBindings.begin(), identityBindings.end(), [&](const auto &binding) {
        return binding.TargetInterfaceId == functionInterfaceId &&
               binding.TargetOperationId == CPlotOperationIdCommandArtistSetFunctionV2;
    }));

    const auto resampleBindings = semanticCatalog.QueryBindingsForOperator(COperatorIdResampleV1);
    CHECK_FALSE(std::any_of(resampleBindings.begin(), resampleBindings.end(), [&](const auto &binding) {
        return binding.TargetInterfaceId == functionInterfaceId &&
               binding.TargetOperationId == CPlotOperationIdCommandArtistDoubleSampleCountV2;
    }));
    CHECK_FALSE(std::any_of(resampleBindings.begin(), resampleBindings.end(), [&](const auto &binding) {
        return binding.TargetInterfaceId == functionInterfaceId &&
               binding.TargetOperationId == CPlotOperationIdCommandArtistSetDomainV2;
    }));

    overlay.Clear();
    semanticCatalog.ClearBindings();
    AttachSemanticsForCatalogV1(
        plotCatalog.GetCatalog(),
        overlay,
        semanticCatalog,
        FAttachSemanticsOptionsV1{
            .bAttachImplementationBindings = true,
            .bIncludeConfigurationOperationBindings = true
        });

    const auto setFunctionSemanticOpOptIn = overlay.FindOperationSemanticOperatorId(
        functionInterfaceId,
        CPlotOperationIdCommandArtistSetFunctionV2);
    REQUIRE(setFunctionSemanticOpOptIn.has_value());
    CHECK(*setFunctionSemanticOpOptIn == COperatorIdIdentityV1);

    const auto identityBindingsOptIn = semanticCatalog.QueryBindingsForOperator(COperatorIdIdentityV1);
    CHECK(std::any_of(identityBindingsOptIn.begin(), identityBindingsOptIn.end(), [&](const auto &binding) {
        return binding.TargetInterfaceId == functionInterfaceId &&
               binding.TargetOperationId == CPlotOperationIdCommandArtistSetFunctionV2;
    }));

    const auto resampleBindingsOptIn = semanticCatalog.QueryBindingsForOperator(COperatorIdResampleV1);
    CHECK(std::any_of(resampleBindingsOptIn.begin(), resampleBindingsOptIn.end(), [&](const auto &binding) {
        return binding.TargetInterfaceId == functionInterfaceId &&
               binding.TargetOperationId == CPlotOperationIdCommandArtistDoubleSampleCountV2;
    }));
    CHECK(std::any_of(resampleBindingsOptIn.begin(), resampleBindingsOptIn.end(), [&](const auto &binding) {
        return binding.TargetInterfaceId == functionInterfaceId &&
               binding.TargetOperationId == CPlotOperationIdCommandArtistSetDomainV2;
    }));
}
