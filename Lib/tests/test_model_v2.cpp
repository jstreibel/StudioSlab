#include <catch2/catch_all.hpp>

#include "Core/Model/V2/ModelAuthoringV2.h"
#include "Core/Model/V2/ModelSeedsV2.h"

namespace {

    auto HasErrorContaining(const Slab::Core::Model::V2::FValidationResultV2 &result, const Slab::Str &needle) -> bool {
        return std::any_of(result.Messages.begin(), result.Messages.end(), [&](const auto &message) {
            return message.Severity == Slab::Core::Model::V2::EValidationSeverityV2::Error &&
                   message.Message.find(needle) != Slab::Str::npos;
        });
    }

    auto HasSemanticDiagnosticContaining(const Slab::Vector<Slab::Core::Model::V2::FSemanticDiagnosticV2> &diagnostics,
                                         const Slab::Str &code,
                                         const Slab::Str &needle) -> bool {
        return std::any_of(diagnostics.begin(), diagnostics.end(), [&](const auto &diagnostic) {
            return diagnostic.Code == code && diagnostic.Message.find(needle) != Slab::Str::npos;
        });
    }

    auto SelectionContains(const std::set<Slab::Str> &keys,
                           const Slab::Core::Model::V2::FSemanticObjectRefV2 &ref) -> bool {
        return keys.contains(Slab::Core::Model::V2::MakeSemanticObjectKeyV2(ref));
    }

} // namespace

TEST_CASE("Model V2 harmonic oscillator seed is well-formed", "[ModelV2]") {
    using namespace Slab::Core::Model::V2;

    const auto model = BuildHarmonicOscillatorModelV2();
    const auto validation = ValidateModelV2(model);

    REQUIRE(validation.IsOk());
    CHECK(model.ModelId == "model.harmonic_oscillator");
    CHECK(model.BaseVocabulary.ActivePresetId == "classical_mechanics");
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
    CHECK(model.BaseVocabulary.ActivePresetId == "relativistic_field_theory");

    const auto *phi = FindDefinitionByIdV2(model, "field.phi");
    REQUIRE(phi != nullptr);
    CHECK(phi->Kind == EDefinitionKindV2::Field);
    REQUIRE(phi->DeclaredType.has_value());
    CHECK(RenderDialectDefinitionV2(*phi, &model).find("\\phi") != Slab::Str::npos);
    CHECK(RenderDialectDefinitionV2(*phi, &model).find("d + 1") != Slab::Str::npos);

    CHECK(FindDefinitionByIdV2(model, "operator.box") == nullptr);
    const auto resolvedVocabulary = ResolveModelBaseVocabularyEntriesV2(model);
    const auto boxIt = std::find_if(resolvedVocabulary.begin(), resolvedVocabulary.end(), [&](const auto &entry) {
        return entry.Entry.EntryId == "vocab.relativistic_field_theory.operator.box";
    });
    REQUIRE(boxIt != resolvedVocabulary.end());
    CHECK(boxIt->OverrideStatus == EVocabularyOverrideStatusV2::BuiltIn);
    REQUIRE(boxIt->Entry.DefinitionKind.has_value());
    CHECK(*boxIt->Entry.DefinitionKind == EDefinitionKindV2::OperatorSymbol);

    const auto *equation = FindRelationByIdV2(model, "relation.klein_gordon.equation");
    REQUIRE(equation != nullptr);
    CHECK(equation->Kind == ERelationKindV2::OperatorEquation);
    CHECK(RenderDialectRelationV2(*equation, &model).find("\\Box \\phi") != Slab::Str::npos);
}

TEST_CASE("Model V2 base vocabulary presets resolve inherited entries", "[ModelV2][Vocabulary]") {
    using namespace Slab::Core::Model::V2;

    const auto &catalog = GetBaseVocabularyPresetCatalogV2();
    REQUIRE(catalog.size() >= 3);

    const auto coreEntries = ResolveBaseVocabularyPresetEntriesV2("core_math");
    CHECK(std::any_of(coreEntries.begin(), coreEntries.end(), [&](const auto &entry) {
        return entry.EntryId == "vocab.core_math.set.reals" && entry.PreferredNotation == "\\mathbb{R}";
    }));
    CHECK(std::any_of(coreEntries.begin(), coreEntries.end(), [&](const auto &entry) {
        return entry.EntryId == "vocab.core_math.set.complexes" && entry.PreferredNotation == "\\mathbb{C}";
    }));
    CHECK(std::any_of(coreEntries.begin(), coreEntries.end(), [&](const auto &entry) {
        return entry.EntryId == "vocab.core_math.set.naturals" && entry.PreferredNotation == "\\mathbb{N}";
    }));

    const auto mechanicsEntries = ResolveBaseVocabularyPresetEntriesV2("classical_mechanics");
    CHECK(std::any_of(mechanicsEntries.begin(), mechanicsEntries.end(), [&](const auto &entry) {
        return entry.EntryId == "vocab.core_math.set.reals";
    }));
    CHECK(std::any_of(mechanicsEntries.begin(), mechanicsEntries.end(), [&](const auto &entry) {
        return entry.EntryId == "vocab.classical_mechanics.notation.ddot";
    }));
    const auto ddotIt = std::find_if(mechanicsEntries.begin(), mechanicsEntries.end(), [&](const auto &entry) {
        return entry.EntryId == "vocab.classical_mechanics.notation.ddot";
    });
    REQUIRE(ddotIt != mechanicsEntries.end());
    CHECK(RenderBaseVocabularyEntryLatexV2(*ddotIt) == "\\ddot{\\mathrm{state}}");

    const auto fieldTheoryEntries = ResolveBaseVocabularyPresetEntriesV2("relativistic_field_theory");
    CHECK(std::any_of(fieldTheoryEntries.begin(), fieldTheoryEntries.end(), [&](const auto &entry) {
        return entry.EntryId == "vocab.relativistic_field_theory.operator.box";
    }));
    CHECK(std::any_of(fieldTheoryEntries.begin(), fieldTheoryEntries.end(), [&](const auto &entry) {
        return entry.EntryId == "vocab.relativistic_field_theory.operator.partial";
    }));
}

TEST_CASE("Model V2 notation context resolves ambient vocabulary before inference", "[ModelV2][Vocabulary]") {
    using namespace Slab::Core::Model::V2;

    const auto model = BuildKleinGordonModelV2();
    const auto context = FNotationContextV2::FromModel(model);

    const auto boxReference = context.FindReference("\\Box");
    REQUIRE(boxReference.has_value());
    CHECK(boxReference->ReferenceId == "vocab.relativistic_field_theory.operator.box");
    const auto resolvedBoxEntry = FindBaseVocabularyEntryByIdV2(model, boxReference->ReferenceId);
    REQUIRE(resolvedBoxEntry.has_value());
    CHECK(RenderBaseVocabularyEntryLatexV2(*resolvedBoxEntry) == "\\Box");

    const auto relation = ParseRelationNotationV2(
        "relation.test.kg",
        ERelationKindV2::OperatorEquation,
        "\\Box \\phi + m^2 \\phi = 0",
        &context);
    REQUIRE(relation.IsOk());
    REQUIRE(relation.Value->Left != nullptr);
    CHECK(relation.Value->Left->Kind == EExpressionKindV2::Binary);
}

TEST_CASE("Model V2 local definitions override ambient vocabulary aliases", "[ModelV2][Vocabulary]") {
    using namespace Slab::Core::Model::V2;

    auto model = BuildKleinGordonModelV2();
    const auto *phi = FindDefinitionByIdV2(model, "field.phi");
    REQUIRE(phi != nullptr);
    REQUIRE(phi->DeclaredType.has_value());

    FDefinitionV2 localBox;
    localBox.DefinitionId = "operator.box.local";
    localBox.Symbol = "Box";
    localBox.PreferredNotation = "\\Box";
    localBox.DisplayName = "Local Box";
    localBox.Kind = EDefinitionKindV2::OperatorSymbol;
    localBox.OperatorStyle = EOperatorApplicationStyleV2::Prefix;
    localBox.DeclaredType = MakeFunctionTypeV2({*phi->DeclaredType}, *phi->DeclaredType);
    localBox.SourceText = "\\Box : (\\mathbb{R}^{d+1} \\to \\mathbb{R}) \\to (\\mathbb{R}^{d+1} \\to \\mathbb{R})";
    localBox.Metadata["base_vocabulary_entry_id"] = "vocab.relativistic_field_theory.operator.box";
    model.Definitions.push_back(localBox);

    const auto context = FNotationContextV2::FromModel(model);
    const auto boxReference = context.FindReference("\\Box");
    REQUIRE(boxReference.has_value());
    CHECK(boxReference->ReferenceId == "operator.box.local");

    const auto resolvedVocabulary = ResolveModelBaseVocabularyEntriesV2(model);
    const auto boxIt = std::find_if(resolvedVocabulary.begin(), resolvedVocabulary.end(), [&](const auto &entry) {
        return entry.Entry.EntryId == "vocab.relativistic_field_theory.operator.box";
    });
    REQUIRE(boxIt != resolvedVocabulary.end());
    CHECK(boxIt->OverrideStatus == EVocabularyOverrideStatusV2::SpecializedLocally);
    CHECK(boxIt->LocalDefinitionId == "operator.box.local");
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

TEST_CASE("Model V2 transactional relation editing previews before apply", "[ModelV2][Editor]") {
    using namespace Slab::Core::Model::V2;

    auto model = BuildHarmonicOscillatorModelV2();
    auto editor = MakeEditorBufferForRelationV2(model, "relation.oscillator.second_order");
    REQUIRE(editor.has_value());

    const auto *originalRelation = FindRelationByIdV2(model, "relation.oscillator.second_order");
    REQUIRE(originalRelation != nullptr);
    CHECK(originalRelation->SourceText == "\\ddot x + \\omega^2 x = 0");

    SetEditorBufferDraftV2(*editor, "\\dot x = p / m");
    REQUIRE(ParseEditorBufferPreviewV2(model, *editor));
    REQUIRE(editor->RelationPreview.has_value());
    CHECK(editor->RelationPreview->InferredClass == ERelationSemanticClassV2::FirstOrderODELike);
    CHECK(editor->RelationPreview->bCanApply);

    const auto *unchangedRelation = FindRelationByIdV2(model, "relation.oscillator.second_order");
    REQUIRE(unchangedRelation != nullptr);
    CHECK(unchangedRelation->SourceText == "\\ddot x + \\omega^2 x = 0");

    FModelChangeRecordV2 changeRecord;
    REQUIRE(ApplyEditorBufferV2(model, *editor, EModelChangeOriginV2::DirectEdit, &changeRecord));
    CHECK(changeRecord.ObjectId == "relation.oscillator.second_order");
    CHECK(changeRecord.PreviousCanonicalNotation == "\\ddot x + \\omega^2 x = 0");
    CHECK(changeRecord.NewCanonicalNotation == "\\dot x = p / m");

    const auto *appliedRelation = FindRelationByIdV2(model, "relation.oscillator.second_order");
    REQUIRE(appliedRelation != nullptr);
    CHECK(appliedRelation->SourceText == "\\dot x = p / m");
}

TEST_CASE("Model V2 transactional definition editing supports apply and revert", "[ModelV2][Editor]") {
    using namespace Slab::Core::Model::V2;

    auto model = BuildHarmonicOscillatorModelV2();
    auto editor = MakeEditorBufferForDefinitionV2(model, "param.m");
    REQUIRE(editor.has_value());

    SetEditorBufferDraftV2(*editor, "m \\in \\mathbb{C}");
    REQUIRE(ParseEditorBufferPreviewV2(model, *editor));
    REQUIRE(editor->DefinitionPreview.has_value());
    CHECK(editor->DefinitionPreview->bCanApply);

    const auto *originalDefinition = FindDefinitionByIdV2(model, "param.m");
    REQUIRE(originalDefinition != nullptr);
    CHECK(originalDefinition->SourceText == "m \\in \\mathbb{R}");

    REQUIRE(RevertEditorBufferV2(model, *editor));
    CHECK(editor->DraftNotation == "m \\in \\mathbb{R}");
    CHECK_FALSE(editor->bDraftDirty);
    CHECK(FindDefinitionByIdV2(model, "param.m")->SourceText == "m \\in \\mathbb{R}");

    SetEditorBufferDraftV2(*editor, "m \\in \\mathbb{C}");
    REQUIRE(ParseEditorBufferPreviewV2(model, *editor));
    FModelChangeRecordV2 changeRecord;
    REQUIRE(ApplyEditorBufferV2(model, *editor, EModelChangeOriginV2::DirectEdit, &changeRecord));
    CHECK(changeRecord.ObjectId == "param.m");

    const auto *appliedDefinition = FindDefinitionByIdV2(model, "param.m");
    REQUIRE(appliedDefinition != nullptr);
    CHECK(appliedDefinition->SourceText == "m \\in \\mathbb{C}");
}

TEST_CASE("Model V2 editor distinguishes parse errors from semantic issues", "[ModelV2][Editor][Diagnostics]") {
    using namespace Slab::Core::Model::V2;

    const auto oscillator = BuildHarmonicOscillatorModelV2();
    auto parseFailure = MakeEditorBufferForRelationV2(oscillator, "relation.oscillator.second_order");
    REQUIRE(parseFailure.has_value());
    SetEditorBufferDraftV2(*parseFailure, "\\frac{x}{y} = 0");
    CHECK_FALSE(ParseEditorBufferPreviewV2(oscillator, *parseFailure));
    REQUIRE(parseFailure->RelationPreview.has_value());
    CHECK_FALSE(parseFailure->RelationPreview->bParseOk);
    CHECK(HasSemanticDiagnosticContaining(parseFailure->RelationPreview->Diagnostics, "parse_error", "derivative"));

    const auto kg = BuildKleinGordonModelV2();
    auto semanticFailure = MakeEditorBufferForRelationV2(kg, "relation.klein_gordon.equation");
    REQUIRE(semanticFailure.has_value());
    SetEditorBufferDraftV2(*semanticFailure, "\\Box \\phi + m^2 \\psi = 0");
    REQUIRE(ParseEditorBufferPreviewV2(kg, *semanticFailure));
    REQUIRE(semanticFailure->RelationPreview.has_value());
    CHECK_FALSE(semanticFailure->RelationPreview->bCanApply);
    CHECK(HasSemanticDiagnosticContaining(semanticFailure->RelationPreview->Diagnostics, "unresolved_symbol", "\\psi"));
    CHECK(HasSemanticDiagnosticContaining(semanticFailure->RelationPreview->Diagnostics, "inferred_undeclared_role", "\\psi"));
    CHECK(HasSemanticDiagnosticContaining(semanticFailure->RelationPreview->Diagnostics, "deferred_semantics", "\\psi"));
}

TEST_CASE("Model V2 draft preview computes semantic delta for edited relations", "[ModelV2][Editor][DraftDelta]") {
    using namespace Slab::Core::Model::V2;

    auto model = BuildHarmonicOscillatorModelV2();
    auto editor = MakeEditorBufferForRelationV2(model, "relation.oscillator.second_order");
    REQUIRE(editor.has_value());

    SetEditorBufferDraftV2(*editor, "\\dot x = p / m");
    REQUIRE(ParseEditorBufferPreviewV2(model, *editor));
    REQUIRE(editor->RelationPreview.has_value());
    REQUIRE(editor->RelationPreview->SemanticDelta.has_value());

    const auto &delta = *editor->RelationPreview->SemanticDelta;
    REQUIRE(delta.CanonicalRelationClass.has_value());
    REQUIRE(delta.DraftRelationClass.has_value());
    CHECK(*delta.CanonicalRelationClass == ERelationSemanticClassV2::SecondOrderODELike);
    CHECK(*delta.DraftRelationClass == ERelationSemanticClassV2::FirstOrderODELike);
    CHECK(std::any_of(delta.DependencyChanges.begin(), delta.DependencyChanges.end(), [](const auto &change) {
        return change.Kind == ESemanticDeltaKindV2::Added && change.Link.Target.ObjectId == "state.p";
    }));
    CHECK(std::any_of(delta.DependencyChanges.begin(), delta.DependencyChanges.end(), [](const auto &change) {
        return change.Kind == ESemanticDeltaKindV2::Removed && change.Link.Target.ObjectId == "param.omega";
    }));
    CHECK(std::any_of(delta.AmbientDependencyChanges.begin(), delta.AmbientDependencyChanges.end(), [](const auto &change) {
        return change.Kind == ESemanticDeltaKindV2::Removed &&
            change.Link.Target.ObjectId == "vocab.classical_mechanics.notation.ddot";
    }));
}

TEST_CASE("Model V2 draft preview exposes unresolved symbol and assumption delta", "[ModelV2][Editor][DraftDelta][Assumptions]") {
    using namespace Slab::Core::Model::V2;

    auto kg = BuildKleinGordonModelV2();
    auto editor = MakeEditorBufferForRelationV2(kg, "relation.klein_gordon.equation");
    REQUIRE(editor.has_value());

    SetEditorBufferDraftV2(*editor, "\\Box \\phi + m^2 \\psi = 0");
    REQUIRE(ParseEditorBufferPreviewV2(kg, *editor));
    REQUIRE(editor->RelationPreview.has_value());
    REQUIRE(editor->RelationPreview->SemanticDelta.has_value());

    const auto &delta = *editor->RelationPreview->SemanticDelta;
    CHECK(delta.CanonicalStatus.UnresolvedSymbolCount == 0);
    CHECK(delta.DraftStatus.UnresolvedSymbolCount == 1);
    CHECK(delta.DraftStatus.SemanticHealth == ESemanticHealthV2::Error);
    CHECK(std::any_of(delta.ReferencedSymbolChanges.begin(), delta.ReferencedSymbolChanges.end(), [](const auto &change) {
        return change.Kind == ESemanticDeltaKindV2::Added &&
            change.Symbol.SymbolText == "\\psi" &&
            !change.Symbol.bResolved;
    }));
    CHECK(std::any_of(delta.AssumptionChanges.begin(), delta.AssumptionChanges.end(), [](const auto &change) {
        return change.Kind == ESemanticDeltaKindV2::Added &&
            change.Assumption.TargetSymbol == "\\psi" &&
            change.Assumption.bWouldCreateDefinition;
    }));
}

TEST_CASE("Model V2 preview overview exposes draft assumptions and materialization objects", "[ModelV2][Editor][DraftObjects]") {
    using namespace Slab::Core::Model::V2;

    const auto kg = BuildKleinGordonModelV2();
    auto editor = MakeEditorBufferForRelationV2(kg, "relation.klein_gordon.equation");
    REQUIRE(editor.has_value());

    SetEditorBufferDraftV2(*editor, "\\Box \\psi + m^2 \\psi = 0");
    REQUIRE(ParseEditorBufferPreviewV2(kg, *editor));
    REQUIRE(editor->RelationPreview.has_value());
    REQUIRE(editor->RelationPreview->SemanticOverview.has_value());

    const auto &draftOverview = *editor->RelationPreview->SemanticOverview;
    const auto assumptionIt = std::find_if(
        editor->RelationPreview->Assumptions.begin(),
        editor->RelationPreview->Assumptions.end(),
        [](const auto &assumption) {
            return assumption.TargetSymbol == "\\psi" && assumption.bWouldCreateDefinition;
        });
    REQUIRE(assumptionIt != editor->RelationPreview->Assumptions.end());

    const auto materializationPreview = BuildAssumptionMaterializationPreviewV2(kg, *assumptionIt, &draftOverview);
    REQUIRE(materializationPreview.ProposedDefinition.has_value());

    const auto *draftAssumptionObject = draftOverview.FindObject(MakeAssumptionObjectRefV2(assumptionIt->AssumptionId));
    REQUIRE(draftAssumptionObject != nullptr);
    CHECK(std::any_of(draftAssumptionObject->TargetLinks.begin(), draftAssumptionObject->TargetLinks.end(), [&](const auto &link) {
        return link.Target.ObjectId == materializationPreview.ProposedDefinition->DefinitionId;
    }));

    const auto *draftDefinitionObject = draftOverview.FindObject(MakeDefinitionObjectRefV2(materializationPreview.ProposedDefinition->DefinitionId));
    REQUIRE(draftDefinitionObject != nullptr);
    CHECK(draftDefinitionObject->OriginDetail == assumptionIt->AssumptionId);
    CHECK(std::any_of(draftDefinitionObject->SourceLinks.begin(), draftDefinitionObject->SourceLinks.end(), [&](const auto &link) {
        return link.Target.ObjectId == assumptionIt->AssumptionId;
    }));
}

TEST_CASE("Model V2 preview diagnostics navigate to draft assumptions", "[ModelV2][Editor][DraftDiagnostics]") {
    using namespace Slab::Core::Model::V2;

    const auto kg = BuildKleinGordonModelV2();
    auto editor = MakeEditorBufferForRelationV2(kg, "relation.klein_gordon.equation");
    REQUIRE(editor.has_value());

    SetEditorBufferDraftV2(*editor, "\\Box \\phi + m^2 \\psi = 0");
    REQUIRE(ParseEditorBufferPreviewV2(kg, *editor));
    REQUIRE(editor->RelationPreview.has_value());
    REQUIRE(editor->RelationPreview->SemanticOverview.has_value());

    const auto &draftOverview = *editor->RelationPreview->SemanticOverview;
    const auto *draftRelationObject = draftOverview.FindObject(MakeRelationObjectRefV2("relation.klein_gordon.equation"));
    REQUIRE(draftRelationObject != nullptr);

    const auto diagnosticIt = std::find_if(draftRelationObject->Diagnostics.begin(), draftRelationObject->Diagnostics.end(), [](const auto &diagnostic) {
        return diagnostic.Diagnostic.Code == "inferred_undeclared_role" &&
            diagnostic.Diagnostic.Message.find("\\psi") != Slab::Str::npos;
    });
    REQUIRE(diagnosticIt != draftRelationObject->Diagnostics.end());
    REQUIRE(diagnosticIt->NavigateTo.has_value());
    CHECK(diagnosticIt->NavigateTo->Kind == ESemanticObjectKindV2::Assumption);
}

TEST_CASE("Model V2 draft selection context includes preview-created definitions", "[ModelV2][Editor][DraftNavigation]") {
    using namespace Slab::Core::Model::V2;

    const auto kg = BuildKleinGordonModelV2();
    auto editor = MakeEditorBufferForRelationV2(kg, "relation.klein_gordon.equation");
    REQUIRE(editor.has_value());

    SetEditorBufferDraftV2(*editor, "\\Box \\psi + m^2 \\psi = 0");
    REQUIRE(ParseEditorBufferPreviewV2(kg, *editor));
    REQUIRE(editor->RelationPreview.has_value());
    REQUIRE(editor->RelationPreview->SemanticOverview.has_value());

    const auto &draftOverview = *editor->RelationPreview->SemanticOverview;
    const auto selection = BuildSemanticSelectionContextV2(
        draftOverview,
        MakeRelationObjectRefV2("relation.klein_gordon.equation"));

    CHECK(SelectionContains(selection.DependencyKeys, MakeDefinitionObjectRefV2("field.psi")));
    CHECK_FALSE(selection.RelatedAssumptionKeys.empty());
}

TEST_CASE("Model V2 draft reference resolution targets preview-created objects", "[ModelV2][Editor][DraftNavigation][References]") {
    using namespace Slab::Core::Model::V2;

    const auto kg = BuildKleinGordonModelV2();
    auto editor = MakeEditorBufferForRelationV2(kg, "relation.klein_gordon.equation");
    REQUIRE(editor.has_value());

    SetEditorBufferDraftV2(*editor, "\\Box \\psi + m^2 \\psi = 0");
    REQUIRE(ParseEditorBufferPreviewV2(kg, *editor));
    REQUIRE(editor->RelationPreview.has_value());
    REQUIRE(editor->RelationPreview->SemanticOverview.has_value());

    const auto &draftOverview = *editor->RelationPreview->SemanticOverview;
    const auto psiIt = std::find_if(
        editor->RelationPreview->ReferencedSymbols.begin(),
        editor->RelationPreview->ReferencedSymbols.end(),
        [](const auto &symbol) { return symbol.SymbolText == "\\psi"; });
    REQUIRE(psiIt != editor->RelationPreview->ReferencedSymbols.end());

    const auto psiTarget = ResolveReferencedSymbolNavigationTargetV2(draftOverview, *psiIt);
    REQUIRE(psiTarget.has_value());
    CHECK(psiTarget->Kind == ESemanticObjectKindV2::Definition);
    CHECK(psiTarget->ObjectId == "field.psi");

    const auto boxIt = std::find_if(
        editor->RelationPreview->ReferencedSymbols.begin(),
        editor->RelationPreview->ReferencedSymbols.end(),
        [](const auto &symbol) { return symbol.ReferenceId == "vocab.relativistic_field_theory.operator.box"; });
    REQUIRE(boxIt != editor->RelationPreview->ReferencedSymbols.end());

    const auto boxTarget = ResolveReferencedSymbolNavigationTargetV2(draftOverview, *boxIt);
    REQUIRE(boxTarget.has_value());
    CHECK(boxTarget->Kind == ESemanticObjectKindV2::VocabularyEntry);
    CHECK(boxTarget->ObjectId == "vocab.relativistic_field_theory.operator.box");
}

TEST_CASE("Model V2 oscillator damping draft preview stays navigable", "[ModelV2][Editor][DraftNavigation][Oscillator]") {
    using namespace Slab::Core::Model::V2;

    const auto oscillator = BuildHarmonicOscillatorModelV2();
    auto editor = MakeEditorBufferForRelationV2(oscillator, "relation.oscillator.second_order");
    REQUIRE(editor.has_value());

    SetEditorBufferDraftV2(*editor, "\\ddot x + \\gamma \\dot x + \\omega^2 x = 0");
    REQUIRE(ParseEditorBufferPreviewV2(oscillator, *editor));
    REQUIRE(editor->RelationPreview.has_value());
    REQUIRE(editor->RelationPreview->SemanticOverview.has_value());

    const auto &draftOverview = *editor->RelationPreview->SemanticOverview;
    const auto gammaIt = std::find_if(
        editor->RelationPreview->ReferencedSymbols.begin(),
        editor->RelationPreview->ReferencedSymbols.end(),
        [](const auto &symbol) { return symbol.SymbolText == "\\gamma"; });
    REQUIRE(gammaIt != editor->RelationPreview->ReferencedSymbols.end());

    const auto gammaUnresolvedDiagnosticCount = std::count_if(
        editor->RelationPreview->Diagnostics.begin(),
        editor->RelationPreview->Diagnostics.end(),
        [](const auto &diagnostic) {
            return diagnostic.Code == "unresolved_symbol" &&
                diagnostic.Message.find("\\gamma") != Slab::Str::npos;
        });
    CHECK(gammaUnresolvedDiagnosticCount == 1);

    const auto *gammaAssumption = Detail::FindAssumptionForReferencedSymbolV2(editor->RelationPreview->Assumptions, *gammaIt);
    REQUIRE(gammaAssumption != nullptr);
    const auto gammaMaterializationPreview = BuildAssumptionMaterializationPreviewV2(oscillator, *gammaAssumption, &draftOverview);
    REQUIRE(gammaMaterializationPreview.ProposedDefinition.has_value());
    CHECK(gammaMaterializationPreview.ProposedDefinition->DefinitionId == "param.gamma");

    const auto gammaTarget = ResolveReferencedSymbolNavigationTargetV2(draftOverview, *gammaIt);
    REQUIRE(gammaTarget.has_value());
    CHECK(gammaTarget->Kind == ESemanticObjectKindV2::Definition);
    CHECK(gammaTarget->ObjectId == "param.gamma");

    const auto selection = BuildSemanticSelectionContextV2(
        draftOverview,
        MakeRelationObjectRefV2("relation.oscillator.second_order"));
    CHECK(SelectionContains(selection.DependencyKeys, MakeDefinitionObjectRefV2("param.gamma")));

    REQUIRE(RevertEditorBufferV2(oscillator, *editor));
    CHECK_FALSE(editor->bPreviewCurrent);
    CHECK_FALSE(editor->RelationPreview.has_value());
    CHECK(editor->DraftNotation == editor->CanonicalNotation);
}

TEST_CASE("Model V2 assumption materialization preview proposes explicit definition", "[ModelV2][Assumptions][Materialization]") {
    using namespace Slab::Core::Model::V2;

    const auto kg = BuildKleinGordonModelV2();
    auto editor = MakeEditorBufferForRelationV2(kg, "relation.klein_gordon.equation");
    REQUIRE(editor.has_value());

    SetEditorBufferDraftV2(*editor, "\\Box \\psi + m^2 \\psi = 0");
    REQUIRE(ParseEditorBufferPreviewV2(kg, *editor));
    REQUIRE(editor->RelationPreview.has_value());

    const auto assumptionIt = std::find_if(
        editor->RelationPreview->Assumptions.begin(),
        editor->RelationPreview->Assumptions.end(),
        [](const auto &assumption) {
            return assumption.TargetSymbol == "\\psi" && assumption.bWouldCreateDefinition;
        });
    REQUIRE(assumptionIt != editor->RelationPreview->Assumptions.end());

    const auto materializationPreview = BuildAssumptionMaterializationPreviewV2(kg, *assumptionIt);
    CHECK(materializationPreview.bAvailable);
    CHECK(materializationPreview.bWouldCreateDefinition);
    CHECK(materializationPreview.LifecyclePhase == EAssumptionLifecyclePhaseV2::Implicit);
    REQUIRE(materializationPreview.ProposedDefinition.has_value());
    CHECK(materializationPreview.SuggestedDefinitionId == "field.psi");
    CHECK(materializationPreview.ProposedDefinition->Metadata.at("accepted_assumption_id") == assumptionIt->AssumptionId);
    CHECK(materializationPreview.OutcomeLabel.find("create a new local definition") != Slab::Str::npos);
}

TEST_CASE("Model V2 direct creation supports damping parameter workflow", "[ModelV2][Editor][Create]") {
    using namespace Slab::Core::Model::V2;

    auto model = BuildHarmonicOscillatorModelV2();

    const auto gammaPreview = PreviewNewDefinitionV2(
        model,
        "param.gamma",
        EDefinitionKindV2::ScalarParameter,
        ECoordinateRoleV2::Generic,
        EOperatorApplicationStyleV2::Prefix,
        "\\gamma \\in \\mathbb{R}",
        "Damping Coefficient");
    REQUIRE(gammaPreview.bParseOk);
    REQUIRE(gammaPreview.bCanApply);
    REQUIRE(gammaPreview.ProposedDefinition.has_value());
    CHECK(gammaPreview.ProposedDefinition->DefinitionId == "param.gamma");
    CHECK(gammaPreview.ProposedDefinition->Symbol == "gamma");

    FModelChangeRecordV2 definitionChange;
    REQUIRE(ApplyNewDefinitionPreviewV2(model, gammaPreview, &definitionChange));
    CHECK(definitionChange.ObjectKind == EModelObjectKindV2::Definition);
    CHECK(definitionChange.ObjectId == "param.gamma");
    REQUIRE(FindDefinitionByIdV2(model, "param.gamma") != nullptr);

    const auto dampedRelationPreview = PreviewNewRelationV2(
        model,
        "relation.oscillator.damped",
        ERelationKindV2::DifferentialEquation,
        "\\ddot x + \\gamma \\dot x + \\omega^2 x = 0",
        "Damped Oscillator");
    REQUIRE(dampedRelationPreview.bParseOk);
    REQUIRE(dampedRelationPreview.bCanApply);
    REQUIRE(dampedRelationPreview.ProposedRelation.has_value());
    CHECK(dampedRelationPreview.InferredClass == ERelationSemanticClassV2::SecondOrderODELike);
    CHECK(std::any_of(
        dampedRelationPreview.ReferencedSymbols.begin(),
        dampedRelationPreview.ReferencedSymbols.end(),
        [](const auto &symbol) { return symbol.ReferenceId == "param.gamma"; }));

    FModelChangeRecordV2 relationChange;
    REQUIRE(ApplyNewRelationPreviewV2(model, dampedRelationPreview, &relationChange));
    CHECK(relationChange.ObjectKind == EModelObjectKindV2::Relation);
    CHECK(relationChange.ObjectId == "relation.oscillator.damped");
    REQUIRE(FindRelationByIdV2(model, "relation.oscillator.damped") != nullptr);

    const auto validation = ValidateModelV2(model);
    REQUIRE(validation.IsOk());

    const auto overview = BuildModelSemanticOverviewV2(model);
    CHECK(overview.Status.UnresolvedSymbolCount == 0);
    const auto *gammaObject = overview.FindObject(MakeDefinitionObjectRefV2("param.gamma"));
    REQUIRE(gammaObject != nullptr);
    CHECK(std::any_of(gammaObject->UsedBy.begin(), gammaObject->UsedBy.end(), [](const auto &link) {
        return link.Target.ObjectId == "relation.oscillator.damped";
    }));
}

TEST_CASE("Model V2 direct creation preview rejects duplicate ids", "[ModelV2][Editor][Create]") {
    using namespace Slab::Core::Model::V2;

    const auto model = BuildHarmonicOscillatorModelV2();

    const auto duplicateDefinitionPreview = PreviewNewDefinitionV2(
        model,
        "state.x",
        EDefinitionKindV2::StateVariable,
        ECoordinateRoleV2::Generic,
        EOperatorApplicationStyleV2::Prefix,
        "x : \\mathbb{R} \\to \\mathbb{R}");
    REQUIRE_FALSE(duplicateDefinitionPreview.bCanApply);
    CHECK(HasSemanticDiagnosticContaining(
        duplicateDefinitionPreview.Diagnostics,
        "duplicate_definition_id",
        "already exists"));

    const auto duplicateRelationPreview = PreviewNewRelationV2(
        model,
        "relation.oscillator.second_order",
        ERelationKindV2::DifferentialEquation,
        "\\ddot x + \\omega^2 x = 0");
    REQUIRE_FALSE(duplicateRelationPreview.bCanApply);
    CHECK(HasSemanticDiagnosticContaining(
        duplicateRelationPreview.Diagnostics,
        "duplicate_relation_id",
        "already exists"));
}

TEST_CASE("Model V2 accepted assumption materializes local definition with provenance", "[ModelV2][Assumptions][Materialization]") {
    using namespace Slab::Core::Model::V2;

    auto kg = BuildKleinGordonModelV2();
    auto editor = MakeEditorBufferForRelationV2(kg, "relation.klein_gordon.equation");
    REQUIRE(editor.has_value());

    SetEditorBufferDraftV2(*editor, "\\Box \\psi + m^2 \\psi = 0");
    REQUIRE(ParseEditorBufferPreviewV2(kg, *editor));
    REQUIRE(editor->RelationPreview.has_value());
    const auto assumptionIt = std::find_if(editor->RelationPreview->Assumptions.begin(), editor->RelationPreview->Assumptions.end(), [](const auto &assumption) {
        return assumption.TargetSymbol == "\\psi" && assumption.bWouldCreateDefinition;
    });
    REQUIRE(assumptionIt != editor->RelationPreview->Assumptions.end());
    const auto acceptedAssumptionId = assumptionIt->AssumptionId;

    FModelChangeRecordV2 assumptionChangeRecord;
    REQUIRE(AcceptAssumptionV2(kg, *assumptionIt, &assumptionChangeRecord));
    CHECK(assumptionChangeRecord.ObjectKind == EModelObjectKindV2::Definition);

    const auto *psiDefinition = FindDefinitionByIdV2(kg, assumptionChangeRecord.ObjectId);
    REQUIRE(psiDefinition != nullptr);
    CHECK(psiDefinition->Metadata.contains("accepted_assumption_id"));
    CHECK(psiDefinition->Metadata.at("accepted_assumption_id") == acceptedAssumptionId);

    REQUIRE(ParseEditorBufferPreviewV2(kg, *editor));
    REQUIRE(editor->RelationPreview.has_value());
    CHECK(editor->RelationPreview->bCanApply);

    FModelChangeRecordV2 relationChangeRecord;
    REQUIRE(ApplyEditorBufferV2(kg, *editor, EModelChangeOriginV2::DirectEdit, &relationChangeRecord));

    const auto overview = BuildModelSemanticOverviewV2(kg);
    const auto *psiObject = overview.FindObject(MakeDefinitionObjectRefV2(assumptionChangeRecord.ObjectId));
    REQUIRE(psiObject != nullptr);
    CHECK(std::any_of(psiObject->SourceLinks.begin(), psiObject->SourceLinks.end(), [&](const auto &link) {
        return link.Target.ObjectId == acceptedAssumptionId;
    }));

    const auto *assumptionObject = overview.FindObject(MakeAssumptionObjectRefV2(acceptedAssumptionId));
    REQUIRE(assumptionObject != nullptr);
    CHECK(std::any_of(assumptionObject->TargetLinks.begin(), assumptionObject->TargetLinks.end(), [&](const auto &link) {
        return link.Target.ObjectId == assumptionChangeRecord.ObjectId;
    }));
}

TEST_CASE("Model V2 semantic report infers oscillator assumptions", "[ModelV2][Inference]") {
    using namespace Slab::Core::Model::V2;

    const auto model = BuildHarmonicOscillatorModelV2();
    const auto report = BuildModelSemanticReportV2(model);

    const auto relationIt = std::find_if(report.Relations.begin(), report.Relations.end(), [&](const auto &relationReport) {
        return relationReport.RelationId == "relation.oscillator.second_order";
    });
    REQUIRE(relationIt != report.Relations.end());
    CHECK(relationIt->InferredClass == ERelationSemanticClassV2::SecondOrderODELike);

    const auto hasXStateAssumption = std::any_of(report.Assumptions.begin(), report.Assumptions.end(), [&](const auto &assumption) {
        return assumption.SourceId == "relation.oscillator.second_order" &&
            assumption.TargetId == "state.x" &&
            assumption.Kind == EAssumptionKindV2::DefinitionRole &&
            assumption.InferredKind.has_value() &&
            *assumption.InferredKind == EDefinitionKindV2::StateVariable;
    });
    CHECK(hasXStateAssumption);

    const auto hasOmegaParameterAssumption = std::any_of(report.Assumptions.begin(), report.Assumptions.end(), [&](const auto &assumption) {
        return assumption.SourceId == "relation.oscillator.second_order" &&
            assumption.TargetId == "param.omega" &&
            assumption.Kind == EAssumptionKindV2::DefinitionRole &&
            assumption.InferredKind.has_value() &&
            *assumption.InferredKind == EDefinitionKindV2::ScalarParameter;
    });
    CHECK(hasOmegaParameterAssumption);
}

TEST_CASE("Model V2 semantic report infers Klein-Gordon assumptions", "[ModelV2][Inference]") {
    using namespace Slab::Core::Model::V2;

    const auto model = BuildKleinGordonModelV2();
    const auto report = BuildModelSemanticReportV2(model);

    const auto relationIt = std::find_if(report.Relations.begin(), report.Relations.end(), [&](const auto &relationReport) {
        return relationReport.RelationId == "relation.klein_gordon.equation";
    });
    REQUIRE(relationIt != report.Relations.end());
    CHECK(relationIt->InferredClass == ERelationSemanticClassV2::FieldEquationLike);

    const auto phiIt = std::find_if(relationIt->ReferencedSymbols.begin(), relationIt->ReferencedSymbols.end(), [&](const auto &symbol) {
        return symbol.ReferenceId == "field.phi";
    });
    REQUIRE(phiIt != relationIt->ReferencedSymbols.end());
    REQUIRE(phiIt->InferredKind.has_value());
    CHECK(*phiIt->InferredKind == EDefinitionKindV2::Field);

    const auto boxIt = std::find_if(relationIt->ReferencedSymbols.begin(), relationIt->ReferencedSymbols.end(), [&](const auto &symbol) {
        return symbol.ReferenceId == "vocab.relativistic_field_theory.operator.box";
    });
    REQUIRE(boxIt != relationIt->ReferencedSymbols.end());
    CHECK(boxIt->Origin == ESemanticOriginV2::BaseVocabulary);
    REQUIRE(boxIt->InferredKind.has_value());
    CHECK(*boxIt->InferredKind == EDefinitionKindV2::OperatorSymbol);

    const auto massIt = std::find_if(relationIt->ReferencedSymbols.begin(), relationIt->ReferencedSymbols.end(), [&](const auto &symbol) {
        return symbol.ReferenceId == "param.m";
    });
    REQUIRE(massIt != relationIt->ReferencedSymbols.end());
    REQUIRE(massIt->InferredKind.has_value());
    CHECK(*massIt->InferredKind == EDefinitionKindV2::ScalarParameter);

    const bool hasBoxAssumption = std::any_of(report.Assumptions.begin(), report.Assumptions.end(), [&](const auto &assumption) {
        return assumption.TargetId == "vocab.relativistic_field_theory.operator.box" ||
            assumption.TargetSymbol == "\\Box";
    });
    CHECK_FALSE(hasBoxAssumption);
}

TEST_CASE("Model V2 semantic report compares declared and inferred roles", "[ModelV2][Inference][Diagnostics]") {
    using namespace Slab::Core::Model::V2;

    auto model = BuildHarmonicOscillatorModelV2();
    auto *x = FindDefinitionByIdV2(model, "state.x");
    REQUIRE(x != nullptr);
    x->Kind = EDefinitionKindV2::ScalarParameter;

    const auto report = BuildModelSemanticReportV2(model);
    const auto reportIt = std::find_if(report.Definitions.begin(), report.Definitions.end(), [&](const auto &definitionReport) {
        return definitionReport.DefinitionId == "state.x";
    });
    REQUIRE(reportIt != report.Definitions.end());
    REQUIRE(reportIt->InferredKind.has_value());
    CHECK(*reportIt->InferredKind == EDefinitionKindV2::StateVariable);
    CHECK(reportIt->bRoleMismatchesDeclared);
    CHECK(HasSemanticDiagnosticContaining(reportIt->Diagnostics, "declared_inferred_mismatch", "disagrees"));
}

TEST_CASE("Model V2 harmonic oscillator report shows ambient classical mechanics vocabulary", "[ModelV2][Vocabulary][Inference]") {
    using namespace Slab::Core::Model::V2;

    const auto model = BuildHarmonicOscillatorModelV2();
    const auto report = BuildModelSemanticReportV2(model);
    const auto relationIt = std::find_if(report.Relations.begin(), report.Relations.end(), [&](const auto &relationReport) {
        return relationReport.RelationId == "relation.oscillator.second_order";
    });
    REQUIRE(relationIt != report.Relations.end());

    const auto dotIt = std::find_if(relationIt->ReferencedSymbols.begin(), relationIt->ReferencedSymbols.end(), [&](const auto &symbol) {
        return symbol.ReferenceId == "vocab.classical_mechanics.notation.ddot";
    });
    REQUIRE(dotIt != relationIt->ReferencedSymbols.end());
    CHECK(dotIt->Origin == ESemanticOriginV2::BaseVocabulary);

    const auto xIt = std::find_if(relationIt->ReferencedSymbols.begin(), relationIt->ReferencedSymbols.end(), [&](const auto &symbol) {
        return symbol.ReferenceId == "state.x";
    });
    REQUIRE(xIt != relationIt->ReferencedSymbols.end());
    CHECK(xIt->Origin == ESemanticOriginV2::LocalDefinition);
}

TEST_CASE("Model V2 definition report exposes ambient type dependencies and unresolved origin", "[ModelV2][Vocabulary][Diagnostics]") {
    using namespace Slab::Core::Model::V2;

    const auto oscillator = BuildHarmonicOscillatorModelV2();
    const auto report = BuildModelSemanticReportV2(oscillator);
    const auto definitionIt = std::find_if(report.Definitions.begin(), report.Definitions.end(), [&](const auto &definitionReport) {
        return definitionReport.DefinitionId == "state.x";
    });
    REQUIRE(definitionIt != report.Definitions.end());
    const auto realSpaceIt = std::find_if(definitionIt->ReferencedSymbols.begin(), definitionIt->ReferencedSymbols.end(), [&](const auto &symbol) {
        return symbol.ReferenceId == "vocab.core_math.set.reals";
    });
    REQUIRE(realSpaceIt != definitionIt->ReferencedSymbols.end());
    CHECK(realSpaceIt->Origin == ESemanticOriginV2::BaseVocabulary);

    const auto kg = BuildKleinGordonModelV2();
    auto preview = MakeEditorBufferForRelationV2(kg, "relation.klein_gordon.equation");
    REQUIRE(preview.has_value());
    SetEditorBufferDraftV2(*preview, "\\Box \\phi + m^2 \\psi = 0");
    REQUIRE(ParseEditorBufferPreviewV2(kg, *preview));
    REQUIRE(preview->RelationPreview.has_value());
    const auto psiIt = std::find_if(preview->RelationPreview->ReferencedSymbols.begin(), preview->RelationPreview->ReferencedSymbols.end(), [&](const auto &symbol) {
        return symbol.SymbolText == "\\psi";
    });
    REQUIRE(psiIt != preview->RelationPreview->ReferencedSymbols.end());
    CHECK(psiIt->Origin == ESemanticOriginV2::Unresolved);
}

TEST_CASE("Model V2 semantic overview exposes oscillator navigation links", "[ModelV2][Navigation]") {
    using namespace Slab::Core::Model::V2;

    const auto model = BuildHarmonicOscillatorModelV2();
    const auto overview = BuildModelSemanticOverviewV2(model);
    const auto xRef = MakeDefinitionObjectRefV2("state.x");
    const auto *xObject = overview.FindObject(xRef);
    REQUIRE(xObject != nullptr);

    CHECK(std::any_of(xObject->AmbientDependencies.begin(), xObject->AmbientDependencies.end(), [](const auto &link) {
        return link.Target.ObjectId == "vocab.core_math.set.reals";
    }));
    CHECK(std::any_of(xObject->UsedBy.begin(), xObject->UsedBy.end(), [](const auto &link) {
        return link.Target.ObjectId == "relation.oscillator.second_order";
    }));
    CHECK(std::any_of(xObject->UsedBy.begin(), xObject->UsedBy.end(), [](const auto &link) {
        return link.Target.ObjectId == "relation.oscillator.energy";
    }));
    CHECK(std::any_of(xObject->RelatedAssumptions.begin(), xObject->RelatedAssumptions.end(), [&](const auto &link) {
        const auto assumptionIt = std::find_if(overview.Report.Assumptions.begin(), overview.Report.Assumptions.end(), [&](const auto &assumption) {
            return assumption.AssumptionId == link.Target.ObjectId;
        });
        return assumptionIt != overview.Report.Assumptions.end() &&
            assumptionIt->TargetId == "state.x";
    }));

    const auto selection = BuildSemanticSelectionContextV2(overview, xRef);
    CHECK(SelectionContains(selection.UsedByKeys, MakeRelationObjectRefV2("relation.oscillator.second_order")));
    CHECK(SelectionContains(selection.UsedByKeys, MakeRelationObjectRefV2("relation.oscillator.energy")));
    CHECK_FALSE(selection.RelatedAssumptionKeys.empty());
}

TEST_CASE("Model V2 relation overview exposes ambient and local dependencies", "[ModelV2][Navigation][Vocabulary]") {
    using namespace Slab::Core::Model::V2;

    const auto oscillator = BuildHarmonicOscillatorModelV2();
    const auto oscillatorOverview = BuildModelSemanticOverviewV2(oscillator);
    const auto *secondOrder = oscillatorOverview.FindObject(MakeRelationObjectRefV2("relation.oscillator.second_order"));
    REQUIRE(secondOrder != nullptr);
    CHECK(std::any_of(secondOrder->DependsOn.begin(), secondOrder->DependsOn.end(), [](const auto &link) {
        return link.Target.ObjectId == "state.x";
    }));
    CHECK(std::any_of(secondOrder->DependsOn.begin(), secondOrder->DependsOn.end(), [](const auto &link) {
        return link.Target.ObjectId == "param.omega";
    }));
    CHECK(std::any_of(secondOrder->AmbientDependencies.begin(), secondOrder->AmbientDependencies.end(), [](const auto &link) {
        return link.Target.ObjectId == "vocab.classical_mechanics.notation.ddot";
    }));

    const auto kg = BuildKleinGordonModelV2();
    const auto kgOverview = BuildModelSemanticOverviewV2(kg);
    const auto *kgEquation = kgOverview.FindObject(MakeRelationObjectRefV2("relation.klein_gordon.equation"));
    REQUIRE(kgEquation != nullptr);
    CHECK(std::any_of(kgEquation->DependsOn.begin(), kgEquation->DependsOn.end(), [](const auto &link) {
        return link.Target.ObjectId == "field.phi";
    }));
    CHECK(std::any_of(kgEquation->DependsOn.begin(), kgEquation->DependsOn.end(), [](const auto &link) {
        return link.Target.ObjectId == "param.m";
    }));
    CHECK(std::any_of(kgEquation->AmbientDependencies.begin(), kgEquation->AmbientDependencies.end(), [](const auto &link) {
        return link.Target.ObjectId == "vocab.relativistic_field_theory.operator.box";
    }));
}

TEST_CASE("Model V2 status summary classifies oscillator and Klein-Gordon", "[ModelV2][Status][Classification]") {
    using namespace Slab::Core::Model::V2;

    const auto oscillatorOverview = BuildModelSemanticOverviewV2(BuildHarmonicOscillatorModelV2());
    CHECK(oscillatorOverview.Status.Classification.ModelClass == "finite-dimensional dynamical system");
    CHECK(oscillatorOverview.Status.Classification.Character == "ODE-like");
    CHECK(oscillatorOverview.Status.ActiveBaseVocabularyId == "classical_mechanics");
    CHECK(oscillatorOverview.Status.UnresolvedSymbolCount == 0);
    CHECK(std::any_of(
        oscillatorOverview.Status.CanonicalStateVariables.begin(),
        oscillatorOverview.Status.CanonicalStateVariables.end(),
        [](const auto &link) { return link.Target.ObjectId == "state.x"; }));
    CHECK(std::any_of(
        oscillatorOverview.Status.CanonicalStateVariables.begin(),
        oscillatorOverview.Status.CanonicalStateVariables.end(),
        [](const auto &link) { return link.Target.ObjectId == "state.p"; }));
    CHECK(std::any_of(
        oscillatorOverview.Status.Parameters.begin(),
        oscillatorOverview.Status.Parameters.end(),
        [](const auto &link) { return link.Target.ObjectId == "param.m"; }));

    const auto kgOverview = BuildModelSemanticOverviewV2(BuildKleinGordonModelV2());
    CHECK(kgOverview.Status.Classification.ModelClass == "scalar field model");
    CHECK(kgOverview.Status.Classification.Character == "PDE-like");
    CHECK(kgOverview.Status.ActiveBaseVocabularyId == "relativistic_field_theory");
    CHECK(kgOverview.Status.UnresolvedSymbolCount == 0);
    CHECK(std::any_of(
        kgOverview.Status.Fields.begin(),
        kgOverview.Status.Fields.end(),
        [](const auto &link) { return link.Target.ObjectId == "field.phi"; }));
    CHECK(std::any_of(
        kgOverview.Status.Operators.begin(),
        kgOverview.Status.Operators.end(),
        [](const auto &link) { return link.Target.ObjectId == "vocab.relativistic_field_theory.operator.box"; }));
}

TEST_CASE("Model V2 vocabulary overview reports ambient usage and local specialization links", "[ModelV2][Navigation][Override]") {
    using namespace Slab::Core::Model::V2;

    const auto oscillatorOverview = BuildModelSemanticOverviewV2(BuildHarmonicOscillatorModelV2());
    const auto *realNumbers = oscillatorOverview.FindObject(MakeVocabularyObjectRefV2("vocab.core_math.set.reals"));
    REQUIRE(realNumbers != nullptr);
    CHECK(realNumbers->bReadonly);
    CHECK(realNumbers->bAmbient);
    CHECK(std::any_of(realNumbers->UsedBy.begin(), realNumbers->UsedBy.end(), [](const auto &link) {
        return link.Target.ObjectId == "state.x";
    }));

    auto kg = BuildKleinGordonModelV2();
    const auto *phi = FindDefinitionByIdV2(kg, "field.phi");
    REQUIRE(phi != nullptr);
    REQUIRE(phi->DeclaredType.has_value());

    FDefinitionV2 localBox;
    localBox.DefinitionId = "operator.box.local";
    localBox.Symbol = "Box";
    localBox.PreferredNotation = "\\Box";
    localBox.DisplayName = "Local Box";
    localBox.Kind = EDefinitionKindV2::OperatorSymbol;
    localBox.OperatorStyle = EOperatorApplicationStyleV2::Prefix;
    localBox.DeclaredType = MakeFunctionTypeV2({*phi->DeclaredType}, *phi->DeclaredType);
    localBox.SourceText = "\\Box : (\\mathbb{R}^{d+1} \\to \\mathbb{R}) \\to (\\mathbb{R}^{d+1} \\to \\mathbb{R})";
    localBox.Metadata["base_vocabulary_entry_id"] = "vocab.relativistic_field_theory.operator.box";
    kg.Definitions.push_back(localBox);

    const auto kgOverview = BuildModelSemanticOverviewV2(kg);
    const auto *boxVocabulary = kgOverview.FindObject(MakeVocabularyObjectRefV2("vocab.relativistic_field_theory.operator.box"));
    REQUIRE(boxVocabulary != nullptr);
    CHECK(boxVocabulary->bLocalOverride);
    CHECK(std::any_of(boxVocabulary->LocalOverrides.begin(), boxVocabulary->LocalOverrides.end(), [](const auto &link) {
        return link.Target.ObjectId == "operator.box.local";
    }));
}

TEST_CASE("Model V2 assumption overview exposes source target and conflict navigation", "[ModelV2][Navigation][Assumptions]") {
    using namespace Slab::Core::Model::V2;

    auto model = BuildHarmonicOscillatorModelV2();
    auto *x = FindDefinitionByIdV2(model, "state.x");
    REQUIRE(x != nullptr);
    x->Kind = EDefinitionKindV2::ScalarParameter;

    const auto overview = BuildModelSemanticOverviewV2(model);
    const auto assumptionIt = std::find_if(overview.Report.Assumptions.begin(), overview.Report.Assumptions.end(), [](const auto &assumption) {
        return assumption.SourceId == "relation.oscillator.second_order" &&
            assumption.TargetId == "state.x" &&
            assumption.Kind == EAssumptionKindV2::DefinitionRole;
    });
    REQUIRE(assumptionIt != overview.Report.Assumptions.end());

    const auto assumptionRef = MakeAssumptionObjectRefV2(assumptionIt->AssumptionId);
    const auto *assumptionObject = overview.FindObject(assumptionRef);
    REQUIRE(assumptionObject != nullptr);
    CHECK(assumptionObject->bConflict);
    CHECK(std::any_of(assumptionObject->SourceLinks.begin(), assumptionObject->SourceLinks.end(), [](const auto &link) {
        return link.Target.ObjectId == "relation.oscillator.second_order";
    }));
    CHECK(std::any_of(assumptionObject->TargetLinks.begin(), assumptionObject->TargetLinks.end(), [](const auto &link) {
        return link.Target.ObjectId == "state.x";
    }));

    const auto selection = BuildSemanticSelectionContextV2(overview, assumptionRef);
    CHECK(SelectionContains(selection.SourceKeys, MakeRelationObjectRefV2("relation.oscillator.second_order")));
    CHECK(SelectionContains(selection.TargetKeys, MakeDefinitionObjectRefV2("state.x")));
    CHECK(SelectionContains(selection.ConflictKeys, MakeDefinitionObjectRefV2("state.x")));
}
