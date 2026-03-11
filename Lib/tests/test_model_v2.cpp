#include <catch2/catch_all.hpp>

#include "Core/Model/V2/ModelSeedsV2.h"

namespace {

    auto HasErrorContaining(const Slab::Core::Model::V2::FValidationResultV2 &result, const Slab::Str &needle) -> bool {
        return std::any_of(result.Messages.begin(), result.Messages.end(), [&](const auto &message) {
            return message.Severity == Slab::Core::Model::V2::EValidationSeverityV2::Error &&
                   message.Message.find(needle) != Slab::Str::npos;
        });
    }

} // namespace

TEST_CASE("Model V2 harmonic oscillator seed is well-formed", "[ModelV2]") {
    using namespace Slab::Core::Model::V2;

    const auto model = BuildHarmonicOscillatorModelV2();
    const auto validation = ValidateModelV2(model);

    REQUIRE(validation.IsOk());
    CHECK(model.ModelId == "model.harmonic_oscillator");
    CHECK(model.Definitions.size() >= 7);
    CHECK(model.Relations.size() >= 5);

    const auto *time = FindDefinitionByIdV2(model, "coord.t");
    REQUIRE(time != nullptr);
    CHECK(time->Kind == EDefinitionKindV2::Coordinate);
    CHECK(time->CoordinateRole == ECoordinateRoleV2::Time);

    const auto *x = FindDefinitionByIdV2(model, "state.x");
    REQUIRE(x != nullptr);
    CHECK(x->Kind == EDefinitionKindV2::StateVariable);
    REQUIRE(x->DeclaredType.has_value());
    CHECK(x->DeclaredType->Kind == ETypeExprKindV2::Function);
    REQUIRE(x->DeclaredType->Codomain != nullptr);
    CHECK(RenderDialectDefinitionV2(*x, &model) == "x : \\mathbb{R} \\to \\mathbb{R}");

    const auto *equation = FindRelationByIdV2(model, "relation.oscillator.second_order");
    REQUIRE(equation != nullptr);
    CHECK(equation->Kind == ERelationKindV2::DifferentialEquation);
    CHECK(RenderDialectRelationV2(*equation, &model).find("\\ddot x") != Slab::Str::npos);
}

TEST_CASE("Model V2 Klein-Gordon seed is well-formed", "[ModelV2]") {
    using namespace Slab::Core::Model::V2;

    const auto model = BuildKleinGordonModelV2();
    const auto validation = ValidateModelV2(model);

    REQUIRE(validation.IsOk());
    CHECK(model.ModelId == "model.klein_gordon");

    const auto *phi = FindDefinitionByIdV2(model, "field.phi");
    REQUIRE(phi != nullptr);
    CHECK(phi->Kind == EDefinitionKindV2::Field);
    REQUIRE(phi->DeclaredType.has_value());
    CHECK(RenderDialectDefinitionV2(*phi, &model).find("\\phi") != Slab::Str::npos);
    CHECK(RenderDialectDefinitionV2(*phi, &model).find("d + 1") != Slab::Str::npos);

    const auto *box = FindDefinitionByIdV2(model, "operator.box");
    REQUIRE(box != nullptr);
    CHECK(box->Kind == EDefinitionKindV2::OperatorSymbol);
    REQUIRE(box->DeclaredType.has_value());
    CHECK(box->DeclaredType->Kind == ETypeExprKindV2::Function);

    const auto *equation = FindRelationByIdV2(model, "relation.klein_gordon.equation");
    REQUIRE(equation != nullptr);
    CHECK(equation->Kind == ERelationKindV2::OperatorEquation);
    CHECK(RenderDialectRelationV2(*equation, &model).find("\\Box \\phi") != Slab::Str::npos);
}

TEST_CASE("Model V2 notation parser supports constrained definition and relation syntax", "[ModelV2][Notation]") {
    using namespace Slab::Core::Model::V2;

    const auto kg = BuildKleinGordonModelV2();
    const auto kgContext = FNotationContextV2::FromModel(kg);

    const auto parsedDefinition = ParseDefinitionNotationV2("\\phi : \\mathbb{R}^{d+1} \\to \\mathbb{R}", &kgContext);
    REQUIRE(parsedDefinition.IsOk());
    REQUIRE(parsedDefinition.Value->DeclaredType.has_value());
    CHECK(parsedDefinition.Value->CanonicalSymbol == "phi");
    CHECK(parsedDefinition.Value->DeclaredType->Kind == ETypeExprKindV2::Function);
    REQUIRE_FALSE(parsedDefinition.Value->DeclaredType->Domain.empty());
    CHECK(RenderDialectTypeExprV2(parsedDefinition.Value->DeclaredType->Domain.front(), &kg).find("d + 1") != Slab::Str::npos);

    const auto oscillator = BuildHarmonicOscillatorModelV2();
    const auto oscillatorContext = FNotationContextV2::FromModel(oscillator);
    const auto parsedRelation = ParseRelationNotationV2(
        "relation.test.second_order",
        ERelationKindV2::DifferentialEquation,
        "\\frac{d^2 x}{d t^2} + \\omega^2 x = 0",
        &oscillatorContext);
    REQUIRE(parsedRelation.IsOk());
    const auto rendered = RenderDialectRelationV2(*parsedRelation.Value, &oscillator);
    const bool bUsesFractionDerivative = rendered.find("\\frac{d^{2} x}{d t^{2}}") != Slab::Str::npos;
    const bool bUsesDotDerivative = rendered.find("\\ddot x") != Slab::Str::npos;
    CHECK((bUsesFractionDerivative || bUsesDotDerivative));

    auto relationOnlyModel = oscillator;
    relationOnlyModel.Relations = {*parsedRelation.Value};
    CHECK(ValidateModelV2(relationOnlyModel).IsOk());
}

TEST_CASE("Model V2 parser rejects unsupported fraction syntax clearly", "[ModelV2][Notation]") {
    using namespace Slab::Core::Model::V2;

    const auto model = BuildHarmonicOscillatorModelV2();
    const auto context = FNotationContextV2::FromModel(model);
    const auto parsed = ParseRelationNotationV2(
        "relation.bad.unsupported_fraction",
        ERelationKindV2::Equation,
        "\\frac{x}{y} = 0",
        &context);

    REQUIRE_FALSE(parsed.IsOk());
    CHECK(parsed.Error.Message.find("derivative") != Slab::Str::npos);
}

TEST_CASE("Model V2 validation catches duplicate ids and undefined references", "[ModelV2][Validation]") {
    using namespace Slab::Core::Model::V2;

    auto model = BuildHarmonicOscillatorModelV2();
    model.Definitions.push_back(model.Definitions.front());

    FRelationV2 brokenRelation;
    brokenRelation.RelationId = "relation.broken.undefined_symbol";
    brokenRelation.Kind = ERelationKindV2::Equation;
    brokenRelation.Left = MakeSymbolExprV2(MakeReferenceV2("state.y", "y"), "y");
    brokenRelation.Right = MakeLiteralExprV2("0", "0");
    brokenRelation.SourceText = "y = 0";
    model.Relations.push_back(std::move(brokenRelation));

    const auto validation = ValidateModelV2(model);
    REQUIRE_FALSE(validation.IsOk());
    CHECK(HasErrorContaining(validation, "duplicate definition id"));
    CHECK(HasErrorContaining(validation, "is undefined"));
}

TEST_CASE("Model V2 validation catches invalid derivative and operator application targets", "[ModelV2][Validation]") {
    using namespace Slab::Core::Model::V2;

    auto oscillator = BuildHarmonicOscillatorModelV2();
    oscillator.Relations.clear();

    FRelationV2 badDerivative;
    badDerivative.RelationId = "relation.bad.derivative_target";
    badDerivative.Kind = ERelationKindV2::DifferentialEquation;
    badDerivative.Left = MakeDerivativeExprV2(
        EDerivativeFlavorV2::Ordinary,
        {MakeReferenceV2("coord.t", "t")},
        MakeSymbolExprV2(MakeReferenceV2("param.m", "m"), "m"),
        "\\dot m");
    badDerivative.Right = MakeLiteralExprV2("0", "0");
    oscillator.Relations.push_back(std::move(badDerivative));

    const auto derivativeValidation = ValidateModelV2(oscillator);
    REQUIRE_FALSE(derivativeValidation.IsOk());
    CHECK(HasErrorContaining(derivativeValidation, "not differentiable"));

    auto kg = BuildKleinGordonModelV2();
    kg.Relations.clear();
    const auto context = FNotationContextV2::FromModel(kg);
    const auto badOperatorRelation = ParseRelationNotationV2(
        "relation.bad.operator_target",
        ERelationKindV2::OperatorEquation,
        "\\Box m = 0",
        &context);
    REQUIRE(badOperatorRelation.IsOk());
    kg.Relations.push_back(*badOperatorRelation.Value);

    const auto operatorValidation = ValidateModelV2(kg);
    REQUIRE_FALSE(operatorValidation.IsOk());
    CHECK(HasErrorContaining(operatorValidation, "argument[0]"));
}
