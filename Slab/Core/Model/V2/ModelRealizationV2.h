#ifndef STUDIOSLAB_MODEL_REALIZATION_V2_H
#define STUDIOSLAB_MODEL_REALIZATION_V2_H

#include "ModelAuthoringV2.h"

namespace Slab::Core::Model::V2 {

    enum class EODERealizationReadinessV2 : unsigned char {
        Blocked,
        Ready
    };

    enum class EODERealizationStrategyV2 : unsigned char {
        None,
        ExplicitFirstOrderSystem
    };

    enum class EODEExplicitRelationSideV2 : unsigned char {
        Left,
        Right
    };

    struct FODERealizationDiagnosticV2 {
        EValidationSeverityV2 Severity = EValidationSeverityV2::Warning;
        Str Code;
        TOptional<FSemanticObjectRefV2> Source;
        Str Message;
    };

    struct FODERealizationSymbolV2 {
        Str DefinitionId;
        Str DisplayLabel;
        Str CanonicalNotation;
    };

    struct FODERealizationSelectedRelationV2 {
        Str RelationId;
        Str DisplayLabel;
        Str CanonicalNotation;
        Str StateDefinitionId;
        Str StateDisplayLabel;
        EODEExplicitRelationSideV2 DerivativeSide = EODEExplicitRelationSideV2::Left;
        Str DerivativeNotation;
        Str ExplicitExpressionNotation;
    };

    struct FODERealizationDescriptorV2 {
        Str ModelId;
        Str ModelName;
        ESemanticHealthV2 SemanticHealth = ESemanticHealthV2::Error;
        Str ModelClass;
        Str ModelCharacter;
        Str ActiveBaseVocabularyId;
        Str ActiveBaseVocabularyName;
        EODERealizationReadinessV2 Readiness = EODERealizationReadinessV2::Blocked;
        EODERealizationStrategyV2 Strategy = EODERealizationStrategyV2::None;
        TOptional<FODERealizationSymbolV2> TimeCoordinate;
        Vector<FODERealizationSymbolV2> StateVariables;
        Vector<FODERealizationSymbolV2> Parameters;
        Vector<FODERealizationSymbolV2> Observables;
        Vector<FODERealizationSelectedRelationV2> SelectedRelations;
        Vector<FODERealizationDiagnosticV2> Diagnostics;

        [[nodiscard]] auto IsReady() const -> bool {
            return Readiness == EODERealizationReadinessV2::Ready;
        }
    };

    inline auto ToString(const EODERealizationReadinessV2 value) -> const char * {
        switch (value) {
            case EODERealizationReadinessV2::Blocked: return "Blocked";
            case EODERealizationReadinessV2::Ready: return "Ready";
        }

        return "Blocked";
    }

    inline auto ToString(const EODERealizationStrategyV2 value) -> const char * {
        switch (value) {
            case EODERealizationStrategyV2::None: return "None";
            case EODERealizationStrategyV2::ExplicitFirstOrderSystem: return "ExplicitFirstOrderSystem";
        }

        return "None";
    }

    inline auto ToString(const EODEExplicitRelationSideV2 value) -> const char * {
        switch (value) {
            case EODEExplicitRelationSideV2::Left: return "Left";
            case EODEExplicitRelationSideV2::Right: return "Right";
        }

        return "Left";
    }

    namespace RealizationDetail {

        struct FExplicitFirstOrderRelationCandidateV2 {
            const FRelationV2 *Relation = nullptr;
            const FRelationSemanticReportV2 *Report = nullptr;
            const FDefinitionV2 *StateDefinition = nullptr;
            EODEExplicitRelationSideV2 DerivativeSide = EODEExplicitRelationSideV2::Left;
            FExpressionPtrV2 DerivativeExpression = nullptr;
            FExpressionPtrV2 ExplicitExpression = nullptr;
        };

        inline auto BuildRealizationDiagnosticKeyV2(const FODERealizationDiagnosticV2 &diagnostic) -> Str {
            const auto sourceKey = diagnostic.Source.has_value()
                ? MakeSemanticObjectKeyV2(*diagnostic.Source)
                : Str("none");
            return diagnostic.Code + "|" + sourceKey + "|" + diagnostic.Message;
        }

        inline auto AppendRealizationDiagnosticUniqueV2(Vector<FODERealizationDiagnosticV2> &diagnostics,
                                                        FODERealizationDiagnosticV2 diagnostic) -> void {
            const auto key = BuildRealizationDiagnosticKeyV2(diagnostic);
            const auto it = std::find_if(diagnostics.begin(), diagnostics.end(), [&](const auto &existing) {
                return BuildRealizationDiagnosticKeyV2(existing) == key;
            });
            if (it != diagnostics.end()) return;
            diagnostics.push_back(std::move(diagnostic));
        }

        inline auto BuildRealizationSymbolFromObjectRefV2(const FModelV2 &model,
                                                          const FModelSemanticOverviewV2 &overview,
                                                          const FSemanticObjectRefV2 &ref)
            -> TOptional<FODERealizationSymbolV2> {
            if (!ref.IsValid()) return std::nullopt;
            if (ref.Kind != ESemanticObjectKindV2::Definition) return std::nullopt;

            const auto *definition = FindDefinitionByIdV2(model, ref.ObjectId);
            if (definition == nullptr) return std::nullopt;

            FODERealizationSymbolV2 symbol;
            symbol.DefinitionId = definition->DefinitionId;
            symbol.DisplayLabel = !definition->DisplayName.empty()
                ? definition->DisplayName
                : (!definition->PreferredNotation.empty() ? definition->PreferredNotation : definition->DefinitionId);
            symbol.CanonicalNotation = MakeCanonicalDefinitionNotationV2(*definition, &model);

            if (const auto *object = overview.FindObject(ref); object != nullptr) {
                if (!object->DisplayLabel.empty()) symbol.DisplayLabel = object->DisplayLabel;
                if (!object->CanonicalNotation.empty()) symbol.CanonicalNotation = object->CanonicalNotation;
            }

            return symbol;
        }

        inline auto BuildRealizationSymbolFromSummaryLinkV2(const FModelV2 &model,
                                                            const FModelSemanticOverviewV2 &overview,
                                                            const FSemanticNavigationLinkV2 &link)
            -> TOptional<FODERealizationSymbolV2> {
            auto symbol = BuildRealizationSymbolFromObjectRefV2(model, overview, link.Target);
            if (!symbol.has_value()) return std::nullopt;
            if (!link.Label.empty()) symbol->DisplayLabel = link.Label;
            return symbol;
        }

        inline auto ExpressionContainsDerivativeV2(const FExpressionPtrV2 &expression) -> bool {
            bool bContainsDerivative = false;
            VisitExpressionDepthFirstV2(expression, [&](const FExpressionV2 &node) {
                if (node.Kind == EExpressionKindV2::Derivative) {
                    bContainsDerivative = true;
                }
            });
            return bContainsDerivative;
        }

        inline auto TryExtractExplicitStateTimeDerivativeV2(const FExpressionPtrV2 &expression,
                                                            const Str &timeCoordinateId)
            -> TOptional<Str> {
            if (expression == nullptr || expression->Kind != EExpressionKindV2::Derivative) return std::nullopt;
            if (expression->DerivativeFlavor != EDerivativeFlavorV2::Ordinary) return std::nullopt;
            if (expression->DerivativeVariables.size() != 1) return std::nullopt;
            if (expression->DerivativeVariables.front().ReferenceId != timeCoordinateId) return std::nullopt;
            if (expression->Children.size() != 1 || expression->Children.front() == nullptr) return std::nullopt;

            const auto &target = expression->Children.front();
            if (target->Kind != EExpressionKindV2::Symbol) return std::nullopt;
            if (!target->Reference.IsBound()) return std::nullopt;
            return target->Reference.ReferenceId;
        }

        inline auto TryMatchExplicitFirstOrderRelationCandidateV2(
            const FModelV2 &model,
            const FRelationV2 &relation,
            const FRelationSemanticReportV2 &report,
            const std::set<Str> &canonicalStateIds,
            const Str &timeCoordinateId) -> TOptional<FExplicitFirstOrderRelationCandidateV2> {
            if (report.InferredClass != ERelationSemanticClassV2::FirstOrderODELike) return std::nullopt;

            const auto tryMatchSide =
                [&](const EODEExplicitRelationSideV2 derivativeSide,
                    const FExpressionPtrV2 &derivativeExpression,
                    const FExpressionPtrV2 &explicitExpression) -> TOptional<FExplicitFirstOrderRelationCandidateV2> {
                    if (ExpressionContainsDerivativeV2(explicitExpression)) return std::nullopt;

                    const auto stateId = TryExtractExplicitStateTimeDerivativeV2(derivativeExpression, timeCoordinateId);
                    if (!stateId.has_value()) return std::nullopt;
                    if (!canonicalStateIds.contains(*stateId)) return std::nullopt;

                    const auto *stateDefinition = FindDefinitionByIdV2(model, *stateId);
                    if (stateDefinition == nullptr || stateDefinition->Kind != EDefinitionKindV2::StateVariable) {
                        return std::nullopt;
                    }

                    return FExplicitFirstOrderRelationCandidateV2{
                        .Relation = &relation,
                        .Report = &report,
                        .StateDefinition = stateDefinition,
                        .DerivativeSide = derivativeSide,
                        .DerivativeExpression = derivativeExpression,
                        .ExplicitExpression = explicitExpression
                    };
                };

            if (const auto match = tryMatchSide(EODEExplicitRelationSideV2::Left, relation.Left, relation.Right);
                match.has_value()) {
                return match;
            }

            return tryMatchSide(EODEExplicitRelationSideV2::Right, relation.Right, relation.Left);
        }

        inline auto BuildSelectedRelationFromCandidateV2(const FModelV2 &model,
                                                         const FModelSemanticOverviewV2 &overview,
                                                         const FExplicitFirstOrderRelationCandidateV2 &candidate)
            -> FODERealizationSelectedRelationV2 {
            FODERealizationSelectedRelationV2 relation;
            relation.RelationId = candidate.Relation != nullptr ? candidate.Relation->RelationId : Str{};
            relation.DisplayLabel = candidate.Relation != nullptr && !candidate.Relation->Name.empty()
                ? candidate.Relation->Name
                : relation.RelationId;
            relation.CanonicalNotation = candidate.Report != nullptr
                ? candidate.Report->CanonicalNotation
                : (candidate.Relation != nullptr ? MakeCanonicalRelationNotationV2(*candidate.Relation, &model) : Str{});
            relation.StateDefinitionId = candidate.StateDefinition != nullptr ? candidate.StateDefinition->DefinitionId : Str{};
            relation.StateDisplayLabel = candidate.StateDefinition != nullptr && !candidate.StateDefinition->DisplayName.empty()
                ? candidate.StateDefinition->DisplayName
                : relation.StateDefinitionId;
            relation.DerivativeSide = candidate.DerivativeSide;
            relation.DerivativeNotation = RenderDialectExpressionV2(candidate.DerivativeExpression, &model);
            relation.ExplicitExpressionNotation = RenderDialectExpressionV2(candidate.ExplicitExpression, &model);

            if (candidate.StateDefinition != nullptr) {
                if (const auto *object = overview.FindObject(MakeDefinitionObjectRefV2(candidate.StateDefinition->DefinitionId));
                    object != nullptr && !object->DisplayLabel.empty()) {
                    relation.StateDisplayLabel = object->DisplayLabel;
                }
            }

            return relation;
        }

        inline auto IsODELikeRelationClassV2(const ERelationSemanticClassV2 relationClass) -> bool {
            return relationClass == ERelationSemanticClassV2::DifferentialEquationLike ||
                relationClass == ERelationSemanticClassV2::FirstOrderODELike ||
                relationClass == ERelationSemanticClassV2::SecondOrderODELike;
        }

        inline auto IsBlockingPendingODEAssumptionV2(const FSemanticAssumptionV2 &assumption) -> bool {
            if (assumption.Status != EAssumptionStatusV2::Implicit) return false;
            if (assumption.bWouldCreateDefinition) return true;
            return assumption.TargetId.empty() && assumption.MaterializedDefinitionId.empty();
        }

    } // namespace RealizationDetail

    inline auto BuildODERealizationDescriptorV2(const FModelV2 &model,
                                                const FModelSemanticOverviewV2 *pOverview = nullptr)
        -> FODERealizationDescriptorV2 {
        FModelSemanticOverviewV2 ownedOverview;
        if (pOverview == nullptr) {
            ownedOverview = BuildModelSemanticOverviewV2(model);
            pOverview = &ownedOverview;
        }

        const auto &overview = *pOverview;
        FODERealizationDescriptorV2 descriptor;
        descriptor.ModelId = model.ModelId;
        descriptor.ModelName = model.Name;
        descriptor.SemanticHealth = overview.Status.SemanticHealth;
        descriptor.ModelClass = overview.Status.Classification.ModelClass;
        descriptor.ModelCharacter = overview.Status.Classification.Character;
        descriptor.ActiveBaseVocabularyId = overview.Status.ActiveBaseVocabularyId;
        descriptor.ActiveBaseVocabularyName = overview.Status.ActiveBaseVocabularyName;

        for (const auto &link : overview.Status.CanonicalStateVariables) {
            if (const auto symbol = RealizationDetail::BuildRealizationSymbolFromSummaryLinkV2(model, overview, link);
                symbol.has_value()) {
                descriptor.StateVariables.push_back(*symbol);
            }
        }
        for (const auto &link : overview.Status.Parameters) {
            if (const auto symbol = RealizationDetail::BuildRealizationSymbolFromSummaryLinkV2(model, overview, link);
                symbol.has_value()) {
                descriptor.Parameters.push_back(*symbol);
            }
        }
        for (const auto &link : overview.Status.Observables) {
            if (const auto symbol = RealizationDetail::BuildRealizationSymbolFromSummaryLinkV2(model, overview, link);
                symbol.has_value()) {
                descriptor.Observables.push_back(*symbol);
            }
        }

        Vector<const FDefinitionV2 *> timeCoordinates;
        for (const auto &definition : model.Definitions) {
            if (definition.Kind != EDefinitionKindV2::Coordinate) continue;
            if (definition.CoordinateRole != ECoordinateRoleV2::Time) continue;
            timeCoordinates.push_back(&definition);
        }

        if (timeCoordinates.size() == 1) {
            const auto *timeCoordinate = timeCoordinates.front();
            descriptor.TimeCoordinate = RealizationDetail::BuildRealizationSymbolFromObjectRefV2(
                model,
                overview,
                MakeDefinitionObjectRefV2(timeCoordinate->DefinitionId));
        } else if (timeCoordinates.empty()) {
            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                descriptor.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Error,
                    .Code = "missing_time_coordinate",
                    .Source = std::nullopt,
                    .Message = "ODE realization requires exactly one canonical time coordinate."
                });
        } else {
            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                descriptor.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Error,
                    .Code = "ambiguous_time_coordinate",
                    .Source = std::nullopt,
                    .Message = "ODE realization found multiple time coordinates; choose one canonical time coordinate before descent."
                });
        }

        if (overview.Status.ValidationErrorCount > 0) {
            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                descriptor.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Error,
                    .Code = "validation_errors_present",
                    .Source = std::nullopt,
                    .Message = "ODE realization is blocked while canonical model validation errors remain."
                });
        }
        if (overview.Status.UnresolvedSymbolCount > 0) {
            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                descriptor.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Error,
                    .Code = "unresolved_symbols_present",
                    .Source = std::nullopt,
                    .Message = "ODE realization is blocked while canonical model contains unresolved symbols."
                });
        }
        const auto blockingPendingAssumptionCount = static_cast<std::size_t>(std::count_if(
            overview.Report.Assumptions.begin(),
            overview.Report.Assumptions.end(),
            [](const auto &assumption) {
                return RealizationDetail::IsBlockingPendingODEAssumptionV2(assumption);
            }));
        if (blockingPendingAssumptionCount > 0) {
            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                descriptor.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Error,
                    .Code = "pending_assumptions_present",
                    .Source = std::nullopt,
                    .Message = "ODE realization is blocked while canonical model still depends on pending semantic assumptions that have not been materialized."
                });
        }
        if (!overview.Status.Fields.empty()) {
            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                descriptor.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Error,
                    .Code = "field_model_out_of_scope",
                    .Source = std::nullopt,
                    .Message = "Field-valued models are out of scope for the first ODE realization descent."
                });
        }
        if (overview.Status.Classification.Character == "PDE-like") {
            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                descriptor.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Error,
                    .Code = "pde_character_out_of_scope",
                    .Source = std::nullopt,
                    .Message = "PDE-like models are intentionally deferred from the first ODE realization path."
                });
        }
        if (descriptor.StateVariables.empty()) {
            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                descriptor.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Error,
                    .Code = "missing_state_variables",
                    .Source = std::nullopt,
                    .Message = "ODE realization requires at least one canonical state variable."
                });
        }

        const bool bHasBlockingDiagnostics = std::any_of(descriptor.Diagnostics.begin(), descriptor.Diagnostics.end(), [](const auto &diagnostic) {
            return diagnostic.Severity == EValidationSeverityV2::Error;
        });
        if (bHasBlockingDiagnostics || !descriptor.TimeCoordinate.has_value()) return descriptor;

        std::set<Str> canonicalStateIds;
        for (const auto &state : descriptor.StateVariables) {
            if (!state.DefinitionId.empty()) canonicalStateIds.insert(state.DefinitionId);
        }

        std::map<Str, Vector<RealizationDetail::FExplicitFirstOrderRelationCandidateV2>> candidatesByStateId;
        std::set<Str> candidateRelationIds;
        for (const auto &relation : model.Relations) {
            const auto *report = Detail::FindRelationSemanticReportV2(overview.Report, relation.RelationId);
            if (report == nullptr) continue;

            if (const auto candidate = RealizationDetail::TryMatchExplicitFirstOrderRelationCandidateV2(
                    model,
                    relation,
                    *report,
                    canonicalStateIds,
                    descriptor.TimeCoordinate->DefinitionId);
                candidate.has_value()) {
                candidatesByStateId[candidate->StateDefinition->DefinitionId].push_back(*candidate);
                candidateRelationIds.insert(relation.RelationId);
            }
        }

        for (const auto &state : descriptor.StateVariables) {
            const auto source = MakeDefinitionObjectRefV2(state.DefinitionId);
            const auto it = candidatesByStateId.find(state.DefinitionId);
            if (it == candidatesByStateId.end() || it->second.empty()) {
                RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                    descriptor.Diagnostics,
                    FODERealizationDiagnosticV2{
                        .Severity = EValidationSeverityV2::Error,
                        .Code = "missing_explicit_first_order_relation",
                        .Source = source,
                        .Message = "No explicit first-order time-derivative relation was found for canonical state '" +
                            state.DisplayLabel + "'."
                    });
                continue;
            }

            if (it->second.size() > 1) {
                RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                    descriptor.Diagnostics,
                    FODERealizationDiagnosticV2{
                        .Severity = EValidationSeverityV2::Error,
                        .Code = "ambiguous_explicit_first_order_relation",
                        .Source = source,
                        .Message = "Multiple explicit first-order realization relations match canonical state '" +
                            state.DisplayLabel + "'."
                    });
                continue;
            }

            descriptor.SelectedRelations.push_back(
                RealizationDetail::BuildSelectedRelationFromCandidateV2(model, overview, it->second.front()));
        }

        for (const auto &relation : model.Relations) {
            if (candidateRelationIds.contains(relation.RelationId)) continue;

            const auto *report = Detail::FindRelationSemanticReportV2(overview.Report, relation.RelationId);
            if (report == nullptr || !RealizationDetail::IsODELikeRelationClassV2(report->InferredClass)) continue;

            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                descriptor.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Warning,
                    .Code = "ignored_ode_like_relation",
                    .Source = MakeRelationObjectRefV2(relation.RelationId),
                    .Message = "ODE-like relation '" +
                        (relation.Name.empty() ? relation.RelationId : relation.Name) +
                        "' is not part of the explicit first-order realization set."
                });
        }

        const bool bReady = std::none_of(descriptor.Diagnostics.begin(), descriptor.Diagnostics.end(), [](const auto &diagnostic) {
            return diagnostic.Severity == EValidationSeverityV2::Error;
        }) && descriptor.SelectedRelations.size() == descriptor.StateVariables.size();

        if (bReady) {
            descriptor.Readiness = EODERealizationReadinessV2::Ready;
            descriptor.Strategy = EODERealizationStrategyV2::ExplicitFirstOrderSystem;
        }

        return descriptor;
    }

} // namespace Slab::Core::Model::V2

#endif // STUDIOSLAB_MODEL_REALIZATION_V2_H
