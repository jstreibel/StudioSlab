#ifndef STUDIOSLAB_MODEL_AUTHORING_V2_H
#define STUDIOSLAB_MODEL_AUTHORING_V2_H

#include "ModelNotationV2.h"

#include <cstdint>
#include <iomanip>
#include <limits>
#include <set>

namespace Slab::Core::Model::V2 {

    enum class ERelationSemanticClassV2 : unsigned char {
        Unknown,
        Algebraic,
        DifferentialEquationLike,
        FirstOrderODELike,
        SecondOrderODELike,
        PDELike,
        FieldEquationLike
    };

    enum class EDefinitionSemanticRoleV2 : unsigned char {
        Unknown,
        ScalarParameterLike,
        CoordinateLike,
        StateLike,
        FieldLike,
        OperatorLike,
        ObservableLike
    };

    enum class EAssumptionKindV2 : unsigned char {
        DefinitionRole,
        DefinitionDependency,
        RelationClass
    };

    struct FSemanticDiagnosticV2 {
        EValidationSeverityV2 Severity = EValidationSeverityV2::Warning;
        Str Code;
        Str EntityId;
        Str Context;
        Str Message;
    };

    struct FReferencedSymbolSemanticV2 {
        Str SymbolText;
        Str ReferenceId;
        bool bResolved = false;
        StrVector UsageLabels;
        TOptional<EDefinitionKindV2> DeclaredKind;
        TOptional<EDefinitionKindV2> InferredKind;
        Vector<Str> InferredArgumentDefinitionIds;
        bool bDeclaredInferredAgreement = false;
        bool bDeclaredInferredMismatch = false;
    };

    struct FSemanticAssumptionV2 {
        Str AssumptionId;
        EAssumptionKindV2 Kind = EAssumptionKindV2::DefinitionRole;
        Str Category;
        Str TargetId;
        Str TargetSymbol;
        Str SourceId;
        Str SourceSignature;
        TOptional<float> Confidence;
        EAssumptionStatusV2 Status = EAssumptionStatusV2::Implicit;
        TOptional<EDefinitionKindV2> DeclaredKind;
        TOptional<EDefinitionKindV2> InferredKind;
        Vector<Str> ProposedArgumentDefinitionIds;
        TOptional<FTypeExprV2> ProposedType;
        bool bWouldCreateDefinition = false;
        bool bMatchesDeclaredRole = false;
        bool bMismatchesDeclaredRole = false;
        Str MaterializedDefinitionId;
        Str Detail;
    };

    struct FDefinitionSemanticReportV2 {
        Str DefinitionId;
        Str CanonicalNotation;
        Str NormalizedInterpretation;
        TOptional<EDefinitionKindV2> InferredKind;
        Vector<Str> InferredArgumentDefinitionIds;
        bool bRoleMatchesDeclared = false;
        bool bRoleMismatchesDeclared = false;
        Vector<Str> Dependencies;
        Vector<Str> UsedByRelationIds;
        Vector<FSemanticDiagnosticV2> Diagnostics;
        Vector<FSemanticAssumptionV2> Assumptions;
    };

    struct FRelationSemanticReportV2 {
        Str RelationId;
        Str CanonicalNotation;
        Str NormalizedInterpretation;
        ERelationSemanticClassV2 InferredClass = ERelationSemanticClassV2::Unknown;
        Vector<FReferencedSymbolSemanticV2> ReferencedSymbols;
        Vector<FSemanticDiagnosticV2> Diagnostics;
        Vector<FSemanticAssumptionV2> Assumptions;
    };

    struct FModelSemanticReportV2 {
        Vector<FDefinitionSemanticReportV2> Definitions;
        Vector<FRelationSemanticReportV2> Relations;
        Vector<FSemanticAssumptionV2> Assumptions;
        Vector<FSemanticDiagnosticV2> Diagnostics;
    };

    struct FDefinitionDraftPreviewV2 {
        Str DefinitionId;
        Str CanonicalNotation;
        Str DraftNotation;
        bool bParseOk = false;
        bool bCanApply = false;
        bool bHasBlockingErrors = false;
        TOptional<FNotationErrorV2> ParseError;
        Str NormalizedProjection;
        Str NormalizedInterpretation;
        TOptional<FDefinitionV2> ProposedDefinition;
        TOptional<EDefinitionKindV2> InferredKind;
        Vector<FReferencedSymbolSemanticV2> ReferencedSymbols;
        Vector<FSemanticDiagnosticV2> Diagnostics;
        Vector<FSemanticAssumptionV2> Assumptions;
    };

    struct FRelationDraftPreviewV2 {
        Str RelationId;
        Str CanonicalNotation;
        Str DraftNotation;
        bool bParseOk = false;
        bool bCanApply = false;
        bool bHasBlockingErrors = false;
        TOptional<FNotationErrorV2> ParseError;
        Str NormalizedProjection;
        Str NormalizedInterpretation;
        ERelationSemanticClassV2 InferredClass = ERelationSemanticClassV2::Unknown;
        TOptional<FRelationV2> ProposedRelation;
        Vector<FReferencedSymbolSemanticV2> ReferencedSymbols;
        Vector<FSemanticDiagnosticV2> Diagnostics;
        Vector<FSemanticAssumptionV2> Assumptions;
    };

    struct FModelEditorBufferV2 {
        EModelObjectKindV2 TargetKind = EModelObjectKindV2::Definition;
        Str TargetId;
        Str CanonicalNotation;
        Str DraftNotation;
        bool bDraftDirty = false;
        bool bPreviewCurrent = false;
        TOptional<FDefinitionDraftPreviewV2> DefinitionPreview;
        TOptional<FRelationDraftPreviewV2> RelationPreview;
    };

    inline auto ToString(const ERelationSemanticClassV2 value) -> const char * {
        switch (value) {
            case ERelationSemanticClassV2::Unknown: return "Unknown";
            case ERelationSemanticClassV2::Algebraic: return "Algebraic";
            case ERelationSemanticClassV2::DifferentialEquationLike: return "DifferentialEquationLike";
            case ERelationSemanticClassV2::FirstOrderODELike: return "FirstOrderODELike";
            case ERelationSemanticClassV2::SecondOrderODELike: return "SecondOrderODELike";
            case ERelationSemanticClassV2::PDELike: return "PDELike";
            case ERelationSemanticClassV2::FieldEquationLike: return "FieldEquationLike";
        }

        return "Unknown";
    }

    inline auto ToString(const EDefinitionSemanticRoleV2 value) -> const char * {
        switch (value) {
            case EDefinitionSemanticRoleV2::Unknown: return "Unknown";
            case EDefinitionSemanticRoleV2::ScalarParameterLike: return "ScalarParameterLike";
            case EDefinitionSemanticRoleV2::CoordinateLike: return "CoordinateLike";
            case EDefinitionSemanticRoleV2::StateLike: return "StateLike";
            case EDefinitionSemanticRoleV2::FieldLike: return "FieldLike";
            case EDefinitionSemanticRoleV2::OperatorLike: return "OperatorLike";
            case EDefinitionSemanticRoleV2::ObservableLike: return "ObservableLike";
        }

        return "Unknown";
    }

    inline auto ToString(const EAssumptionKindV2 value) -> const char * {
        switch (value) {
            case EAssumptionKindV2::DefinitionRole: return "DefinitionRole";
            case EAssumptionKindV2::DefinitionDependency: return "DefinitionDependency";
            case EAssumptionKindV2::RelationClass: return "RelationClass";
        }

        return "Unknown";
    }

    inline auto MakeCanonicalDefinitionNotationV2(const FDefinitionV2 &definition,
                                                  const FModelV2 *model = nullptr) -> Str {
        if (!definition.SourceText.empty()) return definition.SourceText;
        return RenderDialectDefinitionV2(definition, model);
    }

    inline auto MakeCanonicalRelationNotationV2(const FRelationV2 &relation,
                                                const FModelV2 *model = nullptr) -> Str {
        if (!relation.SourceText.empty()) return relation.SourceText;
        return RenderDialectRelationV2(relation, model);
    }

    inline auto StableHashStringV2(const Str &value) -> Str {
        constexpr std::uint64_t Offset = 1469598103934665603ull;
        constexpr std::uint64_t Prime = 1099511628211ull;

        std::uint64_t hash = Offset;
        for (const unsigned char ch : value) {
            hash ^= static_cast<std::uint64_t>(ch);
            hash *= Prime;
        }

        Slab::StringStream ss;
        ss << std::hex << std::setw(16) << std::setfill('0') << hash;
        return ss.str();
    }

    inline auto NormalizeSemanticSymbolKeyV2(const FReferenceV2 &reference) -> Str {
        if (reference.IsBound()) return "ref:" + reference.ReferenceId;
        return "sym:" + NormalizeSymbolAliasV2(reference.DisplayText());
    }

    inline auto DefinitionKindToSemanticRoleV2(const EDefinitionKindV2 kind) -> EDefinitionSemanticRoleV2 {
        switch (kind) {
            case EDefinitionKindV2::ScalarParameter: return EDefinitionSemanticRoleV2::ScalarParameterLike;
            case EDefinitionKindV2::Coordinate: return EDefinitionSemanticRoleV2::CoordinateLike;
            case EDefinitionKindV2::StateVariable: return EDefinitionSemanticRoleV2::StateLike;
            case EDefinitionKindV2::Field: return EDefinitionSemanticRoleV2::FieldLike;
            case EDefinitionKindV2::OperatorSymbol: return EDefinitionSemanticRoleV2::OperatorLike;
            case EDefinitionKindV2::ObservableSymbol: return EDefinitionSemanticRoleV2::ObservableLike;
        }

        return EDefinitionSemanticRoleV2::Unknown;
    }

    inline auto SemanticRoleToDefinitionKindV2(const EDefinitionSemanticRoleV2 role) -> TOptional<EDefinitionKindV2> {
        switch (role) {
            case EDefinitionSemanticRoleV2::ScalarParameterLike: return EDefinitionKindV2::ScalarParameter;
            case EDefinitionSemanticRoleV2::CoordinateLike: return EDefinitionKindV2::Coordinate;
            case EDefinitionSemanticRoleV2::StateLike: return EDefinitionKindV2::StateVariable;
            case EDefinitionSemanticRoleV2::FieldLike: return EDefinitionKindV2::Field;
            case EDefinitionSemanticRoleV2::OperatorLike: return EDefinitionKindV2::OperatorSymbol;
            case EDefinitionSemanticRoleV2::ObservableLike: return EDefinitionKindV2::ObservableSymbol;
            case EDefinitionSemanticRoleV2::Unknown: return std::nullopt;
        }

        return std::nullopt;
    }

    inline auto RoleMatchesDefinitionKindV2(const EDefinitionKindV2 declaredKind,
                                            const EDefinitionSemanticRoleV2 inferredRole) -> bool {
        const auto inferredKind = SemanticRoleToDefinitionKindV2(inferredRole);
        return inferredKind.has_value() && *inferredKind == declaredKind;
    }

    namespace Detail {

        struct FSymbolEvidenceV2 {
            Str Key;
            Str SymbolText;
            Str ReferenceId;
            bool bResolved = false;
            bool bSeenAsArithmeticTerm = false;
            bool bSeenAsFunctionCallee = false;
            bool bSeenAsOperatorCallee = false;
            bool bSeenAsOperatorArgument = false;
            bool bSeenAsDerivativeVariable = false;
            bool bSeenAsDerivativeTargetOrdinary = false;
            bool bSeenAsDerivativeTargetPartial = false;
            bool bSeenInTypeDimension = false;
            std::set<Str> InferredArgumentDefinitionIds;
            std::set<Str> UsageLabels;
        };

        struct FRelationStructureV2 {
            int MaxOrdinaryDerivativeOrder = 0;
            int MaxPartialDerivativeOrder = 0;
            bool bUsesOperatorApplication = false;
            bool bUsesFieldLikeUsage = false;
        };

        inline auto InsertUniqueStringV2(Vector<Str> &values, const Str &value) -> void {
            if (value.empty()) return;
            if (values.end() != std::find(values.begin(), values.end(), value)) return;
            values.push_back(value);
        }

        inline auto RegisterEvidenceV2(std::map<Str, FSymbolEvidenceV2> &evidenceByKey,
                                       const FReferenceV2 &reference) -> FSymbolEvidenceV2 & {
            const auto key = NormalizeSemanticSymbolKeyV2(reference);
            auto &entry = evidenceByKey[key];
            entry.Key = key;
            entry.SymbolText = reference.DisplayText();
            if (entry.SymbolText.empty()) entry.SymbolText = reference.SymbolText;
            entry.ReferenceId = reference.ReferenceId;
            entry.bResolved = reference.IsBound();
            return entry;
        }

        inline auto CollectTypeDimensionEvidenceV2(const FExpressionPtrV2 &expression,
                                                   std::map<Str, FSymbolEvidenceV2> &evidenceByKey) -> void {
            if (expression == nullptr) return;

            VisitExpressionDepthFirstV2(expression, [&](const FExpressionV2 &node) {
                if (node.Kind != EExpressionKindV2::Symbol &&
                    node.Kind != EExpressionKindV2::FunctionApplication &&
                    node.Kind != EExpressionKindV2::OperatorApplication) {
                    return;
                }

                auto &entry = RegisterEvidenceV2(evidenceByKey, node.Reference);
                entry.bSeenInTypeDimension = true;
                entry.UsageLabels.insert("type-dimension");
            });
        }

        inline auto CollectTypeEvidenceV2(const FTypeExprV2 &type,
                                          std::map<Str, FSymbolEvidenceV2> &evidenceByKey) -> void {
            if (type.Kind == ETypeExprKindV2::Space) {
                if (type.Space.Dimension.has_value()) CollectTypeDimensionEvidenceV2(*type.Space.Dimension, evidenceByKey);
                return;
            }

            for (const auto &domainType : type.Domain) {
                CollectTypeEvidenceV2(domainType, evidenceByKey);
            }
            if (type.Codomain != nullptr) CollectTypeEvidenceV2(*type.Codomain, evidenceByKey);
        }

        inline auto MaybeMarkCoordinateArgumentsV2(FSymbolEvidenceV2 &entry,
                                                   const Vector<FExpressionPtrV2> &arguments,
                                                   const FModelV2 &model) -> void {
            for (const auto &argument : arguments) {
                if (argument == nullptr || argument->Kind != EExpressionKindV2::Symbol || !argument->Reference.IsBound()) continue;
                const auto *definition = FindDefinitionByIdV2(model, argument->Reference.ReferenceId);
                if (definition == nullptr || definition->Kind != EDefinitionKindV2::Coordinate) continue;
                entry.InferredArgumentDefinitionIds.insert(definition->DefinitionId);
            }
        }

        inline auto CollectExpressionEvidenceV2(const FExpressionPtrV2 &expression,
                                                const FModelV2 &model,
                                                std::map<Str, FSymbolEvidenceV2> &evidenceByKey,
                                                FRelationStructureV2 &structure) -> void {
            if (expression == nullptr) return;

            switch (expression->Kind) {
                case EExpressionKindV2::Symbol: {
                    auto &entry = RegisterEvidenceV2(evidenceByKey, expression->Reference);
                    entry.bSeenAsArithmeticTerm = true;
                    entry.UsageLabels.insert("symbol");
                    return;
                }
                case EExpressionKindV2::Literal:
                    return;
                case EExpressionKindV2::Unary:
                case EExpressionKindV2::Binary:
                    for (const auto &child : expression->Children) {
                        CollectExpressionEvidenceV2(child, model, evidenceByKey, structure);
                    }
                    return;
                case EExpressionKindV2::FunctionApplication: {
                    auto &entry = RegisterEvidenceV2(evidenceByKey, expression->Reference);
                    entry.bSeenAsFunctionCallee = true;
                    entry.UsageLabels.insert("function");
                    MaybeMarkCoordinateArgumentsV2(entry, expression->Children, model);
                    for (const auto &child : expression->Children) {
                        CollectExpressionEvidenceV2(child, model, evidenceByKey, structure);
                    }
                    return;
                }
                case EExpressionKindV2::OperatorApplication: {
                    auto &entry = RegisterEvidenceV2(evidenceByKey, expression->Reference);
                    entry.bSeenAsOperatorCallee = true;
                    entry.UsageLabels.insert("operator");
                    structure.bUsesOperatorApplication = true;
                    for (const auto &child : expression->Children) {
                        if (child != nullptr &&
                            (child->Kind == EExpressionKindV2::Symbol || child->Kind == EExpressionKindV2::FunctionApplication)) {
                            auto &argumentEntry = RegisterEvidenceV2(evidenceByKey, child->Reference);
                            argumentEntry.bSeenAsOperatorArgument = true;
                            argumentEntry.UsageLabels.insert("operator-argument");
                            structure.bUsesFieldLikeUsage = true;
                        }
                        CollectExpressionEvidenceV2(child, model, evidenceByKey, structure);
                    }
                    return;
                }
                case EExpressionKindV2::Derivative: {
                    const auto order = static_cast<int>(expression->DerivativeVariables.size());
                    if (expression->DerivativeFlavor == EDerivativeFlavorV2::Ordinary) {
                        structure.MaxOrdinaryDerivativeOrder = std::max(structure.MaxOrdinaryDerivativeOrder, order);
                    } else {
                        structure.MaxPartialDerivativeOrder = std::max(structure.MaxPartialDerivativeOrder, order);
                        structure.bUsesFieldLikeUsage = true;
                    }

                    for (const auto &variable : expression->DerivativeVariables) {
                        auto &variableEntry = RegisterEvidenceV2(evidenceByKey, variable);
                        variableEntry.bSeenAsDerivativeVariable = true;
                        variableEntry.UsageLabels.insert("derivative-variable");
                    }

                    if (!expression->Children.empty() && expression->Children.front() != nullptr) {
                        const auto &target = expression->Children.front();
                        if (target->Kind == EExpressionKindV2::Symbol ||
                            target->Kind == EExpressionKindV2::FunctionApplication) {
                            auto &targetEntry = RegisterEvidenceV2(evidenceByKey, target->Reference);
                            if (expression->DerivativeFlavor == EDerivativeFlavorV2::Ordinary) {
                                targetEntry.bSeenAsDerivativeTargetOrdinary = true;
                                targetEntry.UsageLabels.insert("ordinary-derivative-target");
                            } else {
                                targetEntry.bSeenAsDerivativeTargetPartial = true;
                                targetEntry.UsageLabels.insert("partial-derivative-target");
                                structure.bUsesFieldLikeUsage = true;
                            }

                            for (const auto &variable : expression->DerivativeVariables) {
                                if (variable.IsBound()) targetEntry.InferredArgumentDefinitionIds.insert(variable.ReferenceId);
                            }
                        }

                        CollectExpressionEvidenceV2(target, model, evidenceByKey, structure);
                    }
                    return;
                }
            }
        }

        inline auto ChooseInferredRoleV2(const FSymbolEvidenceV2 &evidence) -> EDefinitionSemanticRoleV2 {
            if (evidence.bSeenAsDerivativeVariable) return EDefinitionSemanticRoleV2::CoordinateLike;
            if (evidence.bSeenAsOperatorCallee) return EDefinitionSemanticRoleV2::OperatorLike;
            if (evidence.bSeenAsDerivativeTargetPartial || evidence.bSeenAsOperatorArgument) {
                return EDefinitionSemanticRoleV2::FieldLike;
            }
            if (evidence.bSeenAsFunctionCallee || evidence.bSeenAsDerivativeTargetOrdinary) {
                return EDefinitionSemanticRoleV2::StateLike;
            }
            if (evidence.bSeenInTypeDimension || evidence.bSeenAsArithmeticTerm) {
                return EDefinitionSemanticRoleV2::ScalarParameterLike;
            }
            return EDefinitionSemanticRoleV2::Unknown;
        }

        inline auto MakeDiagnosticCodeFromValidationMessageV2(const FValidationMessageV2 &message) -> Str {
            if (message.Message.find("unresolved") != Str::npos) return "unresolved_symbol";
            if (message.Message.find("non-function") != Str::npos ||
                message.Message.find("non-operator") != Str::npos ||
                message.Message.find("incompatible") != Str::npos ||
                message.Message.find("expects ") != Str::npos ||
                message.Message.find("not callable") != Str::npos ||
                message.Message.find("not differentiable") != Str::npos ||
                message.Message.find("argument[") != Str::npos) {
                return "incompatible_symbol_use";
            }
            return "validation";
        }

        inline auto ConvertValidationDiagnosticsV2(const Vector<FValidationMessageV2> &messages) -> Vector<FSemanticDiagnosticV2> {
            Vector<FSemanticDiagnosticV2> diagnostics;
            diagnostics.reserve(messages.size());
            for (const auto &message : messages) {
                diagnostics.push_back(FSemanticDiagnosticV2{
                    .Severity = message.Severity,
                    .Code = MakeDiagnosticCodeFromValidationMessageV2(message),
                    .EntityId = message.EntityId,
                    .Context = message.Context,
                    .Message = message.Message
                });
            }
            return diagnostics;
        }

        inline auto AppendDiagnosticIfMissingV2(Vector<FSemanticDiagnosticV2> &diagnostics,
                                                FSemanticDiagnosticV2 diagnostic) -> void {
            const auto it = std::find_if(diagnostics.begin(), diagnostics.end(), [&](const auto &existing) {
                return existing.Code == diagnostic.Code &&
                    existing.Context == diagnostic.Context &&
                    existing.Message == diagnostic.Message;
            });
            if (it != diagnostics.end()) return;
            diagnostics.push_back(std::move(diagnostic));
        }

        inline auto BuildProposedTypeForAssumptionV2(const FModelV2 &model,
                                                     const EDefinitionSemanticRoleV2 role,
                                                     const Vector<Str> &argumentDefinitionIds) -> TOptional<FTypeExprV2> {
            if (role == EDefinitionSemanticRoleV2::CoordinateLike ||
                role == EDefinitionSemanticRoleV2::ScalarParameterLike) {
                return MakeSpaceTypeV2(MakeRealSpaceV2());
            }

            if (role == EDefinitionSemanticRoleV2::OperatorLike) {
                for (const auto &definition : model.Definitions) {
                    if (definition.Kind == EDefinitionKindV2::Field && definition.DeclaredType.has_value()) {
                        return MakeFunctionTypeV2({*definition.DeclaredType}, *definition.DeclaredType);
                    }
                }

                const auto scalarFunction = MakeFunctionTypeV2({MakeSpaceTypeV2(MakeRealSpaceV2())}, MakeSpaceTypeV2(MakeRealSpaceV2()));
                return MakeFunctionTypeV2({scalarFunction}, scalarFunction);
            }

            Vector<FTypeExprV2> domain;
            for (const auto &argumentId : argumentDefinitionIds) {
                const auto *argumentDefinition = FindDefinitionByIdV2(model, argumentId);
                if (argumentDefinition == nullptr || !argumentDefinition->DeclaredType.has_value()) continue;
                domain.push_back(*argumentDefinition->DeclaredType);
            }

            if (domain.empty()) {
                if (role == EDefinitionSemanticRoleV2::StateLike) {
                    const auto timeIt = std::find_if(model.Definitions.begin(), model.Definitions.end(), [](const auto &definition) {
                        return definition.Kind == EDefinitionKindV2::Coordinate &&
                            definition.CoordinateRole == ECoordinateRoleV2::Time &&
                            definition.DeclaredType.has_value();
                    });
                    if (timeIt != model.Definitions.end()) {
                        domain.push_back(*timeIt->DeclaredType);
                    }
                } else if (role == EDefinitionSemanticRoleV2::FieldLike) {
                    for (const auto &definition : model.Definitions) {
                        if (definition.Kind == EDefinitionKindV2::Field && definition.DeclaredType.has_value()) {
                            return definition.DeclaredType;
                        }
                    }
                }
            }

            if (domain.empty()) {
                domain.push_back(MakeSpaceTypeV2(MakeRealSpaceV2()));
            }

            return MakeFunctionTypeV2(domain, MakeSpaceTypeV2(MakeRealSpaceV2()));
        }

        inline auto MakeRelationSourceSignatureV2(const FRelationSemanticReportV2 &report) -> Str {
            return report.RelationId + ":" + StableHashStringV2(report.CanonicalNotation);
        }

        inline auto MakeRoleAssumptionIdV2(const Str &sourceSignature,
                                           const FReferencedSymbolSemanticV2 &symbol) -> Str {
            const auto roleLabel = symbol.InferredKind.has_value() ? ToString(*symbol.InferredKind) : "unknown";
            return "assumption.role." + sourceSignature + "." + NormalizeSymbolAliasV2(symbol.SymbolText) + "." + roleLabel;
        }

        inline auto MakeDependencyAssumptionIdV2(const Str &sourceSignature,
                                                 const FReferencedSymbolSemanticV2 &symbol) -> Str {
            return "assumption.dependency." + sourceSignature + "." + NormalizeSymbolAliasV2(symbol.SymbolText);
        }

        inline auto MakeRelationClassAssumptionIdV2(const Str &sourceSignature,
                                                    const FRelationSemanticReportV2 &report) -> Str {
            return "assumption.relation_class." + sourceSignature + "." + ToString(report.InferredClass);
        }

        inline auto FindPersistedAssumptionStateV2(const FModelV2 &model,
                                                   const Str &assumptionId) -> const FAssumptionStateV2 * {
            return FindAssumptionStateByIdV2(model, assumptionId);
        }

        inline auto ApplyPersistedAssumptionStateV2(const FModelV2 &model,
                                                    FSemanticAssumptionV2 &assumption) -> void {
            if (const auto *state = FindPersistedAssumptionStateV2(model, assumption.AssumptionId); state != nullptr) {
                assumption.Status = state->Status;
                assumption.MaterializedDefinitionId = state->MaterializedDefinitionId;
            }
        }

        inline auto RenderRoleInterpretationV2(const EDefinitionSemanticRoleV2 role,
                                               const Vector<Str> &argumentIds) -> Str {
            switch (role) {
                case EDefinitionSemanticRoleV2::ScalarParameterLike:
                    return "Scalar parameter-like symbol.";
                case EDefinitionSemanticRoleV2::CoordinateLike:
                    return "Coordinate-like symbol.";
                case EDefinitionSemanticRoleV2::StateLike:
                    if (!argumentIds.empty()) return "State-like quantity depending on " + argumentIds.front() + ".";
                    return "State-like quantity.";
                case EDefinitionSemanticRoleV2::FieldLike:
                    return "Field-like quantity over a symbolic domain.";
                case EDefinitionSemanticRoleV2::OperatorLike:
                    return "Operator-like symbol applied to other model objects.";
                case EDefinitionSemanticRoleV2::ObservableLike:
                    return "Observable-like quantity.";
                case EDefinitionSemanticRoleV2::Unknown:
                    return "No stable inferred role.";
            }

            return "No stable inferred role.";
        }

        inline auto RenderRelationInterpretationV2(const ERelationSemanticClassV2 relationClass) -> Str {
            switch (relationClass) {
                case ERelationSemanticClassV2::Unknown: return "Unclassified relation.";
                case ERelationSemanticClassV2::Algebraic: return "Algebraic relation.";
                case ERelationSemanticClassV2::DifferentialEquationLike: return "Differential-equation-like relation.";
                case ERelationSemanticClassV2::FirstOrderODELike: return "First-order ODE-like relation.";
                case ERelationSemanticClassV2::SecondOrderODELike: return "Second-order ODE-like relation.";
                case ERelationSemanticClassV2::PDELike: return "PDE-like relation.";
                case ERelationSemanticClassV2::FieldEquationLike: return "Field-equation / PDE-like relation.";
            }

            return "Unclassified relation.";
        }

        inline auto ClassifyRelationV2(const FRelationStructureV2 &structure) -> ERelationSemanticClassV2 {
            if (structure.bUsesOperatorApplication || structure.MaxPartialDerivativeOrder > 0) {
                if (structure.bUsesFieldLikeUsage || structure.MaxPartialDerivativeOrder > 0) {
                    return ERelationSemanticClassV2::FieldEquationLike;
                }
                return ERelationSemanticClassV2::PDELike;
            }

            if (structure.MaxOrdinaryDerivativeOrder >= 2) return ERelationSemanticClassV2::SecondOrderODELike;
            if (structure.MaxOrdinaryDerivativeOrder == 1) return ERelationSemanticClassV2::FirstOrderODELike;
            if (structure.MaxOrdinaryDerivativeOrder > 0) return ERelationSemanticClassV2::DifferentialEquationLike;
            return ERelationSemanticClassV2::Algebraic;
        }

        inline auto BuildReferencedSymbolSemanticV2(const FModelV2 &model,
                                                    const FSymbolEvidenceV2 &evidence) -> FReferencedSymbolSemanticV2 {
            FReferencedSymbolSemanticV2 symbol;
            symbol.SymbolText = evidence.SymbolText;
            symbol.ReferenceId = evidence.ReferenceId;
            symbol.bResolved = evidence.bResolved;
            symbol.UsageLabels.assign(evidence.UsageLabels.begin(), evidence.UsageLabels.end());
            symbol.InferredArgumentDefinitionIds.assign(
                evidence.InferredArgumentDefinitionIds.begin(),
                evidence.InferredArgumentDefinitionIds.end());

            const auto inferredRole = ChooseInferredRoleV2(evidence);
            symbol.InferredKind = SemanticRoleToDefinitionKindV2(inferredRole);
            if (evidence.bResolved) {
                if (const auto *definition = FindDefinitionByIdV2(model, evidence.ReferenceId); definition != nullptr) {
                    symbol.DeclaredKind = definition->Kind;
                    if (symbol.InferredKind.has_value()) {
                        symbol.bDeclaredInferredAgreement = *symbol.InferredKind == definition->Kind;
                        symbol.bDeclaredInferredMismatch = *symbol.InferredKind != definition->Kind;
                    }
                }
            }
            return symbol;
        }

        inline auto AppendInferenceDiagnosticsForSymbolV2(const FReferencedSymbolSemanticV2 &symbol,
                                                          const Str &entityId,
                                                          const char *context,
                                                          Vector<FSemanticDiagnosticV2> &diagnostics) -> void {
            if (!symbol.bResolved && symbol.InferredKind.has_value()) {
                AppendDiagnosticIfMissingV2(
                    diagnostics,
                    FSemanticDiagnosticV2{
                        .Severity = EValidationSeverityV2::Warning,
                        .Code = "inferred_undeclared_role",
                        .EntityId = entityId,
                        .Context = context,
                        .Message = "Inferred " + Str(ToString(*symbol.InferredKind)) +
                            " role for undeclared symbol '" + symbol.SymbolText + "'."
                    });
                AppendDiagnosticIfMissingV2(
                    diagnostics,
                    FSemanticDiagnosticV2{
                        .Severity = EValidationSeverityV2::Warning,
                        .Code = "deferred_semantics",
                        .EntityId = entityId,
                        .Context = context,
                        .Message = "Symbol '" + symbol.SymbolText +
                            "' remains implicit until the assumption is accepted into canonical model state."
                    });
            }

            if (symbol.bDeclaredInferredMismatch && symbol.DeclaredKind.has_value() && symbol.InferredKind.has_value()) {
                AppendDiagnosticIfMissingV2(
                    diagnostics,
                    FSemanticDiagnosticV2{
                        .Severity = EValidationSeverityV2::Warning,
                        .Code = "declared_inferred_mismatch",
                        .EntityId = entityId,
                        .Context = context,
                        .Message = "Declared role '" + Str(ToString(*symbol.DeclaredKind)) +
                            "' disagrees with inferred role '" + Str(ToString(*symbol.InferredKind)) +
                            "' for '" + symbol.SymbolText + "'."
                    });
            }
        }

        inline auto BuildDefinitionIdSuggestionV2(const FModelV2 &model,
                                                  const EDefinitionKindV2 kind,
                                                  const Str &symbolText) -> Str {
            Str prefix = "symbol";
            switch (kind) {
                case EDefinitionKindV2::ScalarParameter: prefix = "param"; break;
                case EDefinitionKindV2::Coordinate: prefix = "coord"; break;
                case EDefinitionKindV2::StateVariable: prefix = "state"; break;
                case EDefinitionKindV2::Field: prefix = "field"; break;
                case EDefinitionKindV2::OperatorSymbol: prefix = "operator"; break;
                case EDefinitionKindV2::ObservableSymbol: prefix = "obs"; break;
            }

            const auto baseAlias = NormalizeSymbolAliasV2(symbolText).empty()
                ? Str("symbol")
                : NormalizeSymbolAliasV2(symbolText);
            Str candidate = prefix + "." + baseAlias;
            int suffix = 2;
            while (FindDefinitionByIdV2(model, candidate) != nullptr) {
                candidate = prefix + "." + baseAlias + "_" + ToStr(suffix++);
            }
            return candidate;
        }

        inline auto EnsureAssumptionStateV2(FModelV2 &model, const Str &assumptionId) -> FAssumptionStateV2 & {
            if (auto *existing = FindAssumptionStateByIdV2(model, assumptionId); existing != nullptr) return *existing;
            model.AssumptionStates.push_back(FAssumptionStateV2{.AssumptionId = assumptionId});
            return model.AssumptionStates.back();
        }

        inline auto BuildDefinitionReportFromModelV2(const FModelV2 &model,
                                                     const FDefinitionV2 &definition,
                                                     const Vector<FValidationMessageV2> &validationMessages,
                                                     const std::map<Str, FSymbolEvidenceV2> &globalEvidenceByKey,
                                                     const std::map<Str, Vector<FSemanticAssumptionV2>> &assumptionsByTargetId,
                                                     const std::map<Str, Vector<Str>> &usedByRelationsByDefinitionId) -> FDefinitionSemanticReportV2 {
            FDefinitionSemanticReportV2 report;
            report.DefinitionId = definition.DefinitionId;
            report.CanonicalNotation = MakeCanonicalDefinitionNotationV2(definition, &model);
            report.Dependencies = definition.DependencyDefinitionIds;

            if (const auto usedIt = usedByRelationsByDefinitionId.find(definition.DefinitionId);
                usedIt != usedByRelationsByDefinitionId.end()) {
                report.UsedByRelationIds = usedIt->second;
            }

            const auto evidenceKey = NormalizeSemanticSymbolKeyV2(MakeReferenceV2(definition.DefinitionId, RenderDefinitionLabelV2(definition)));
            if (const auto evidenceIt = globalEvidenceByKey.find(evidenceKey); evidenceIt != globalEvidenceByKey.end()) {
                const auto inferredRole = ChooseInferredRoleV2(evidenceIt->second);
                report.InferredKind = SemanticRoleToDefinitionKindV2(inferredRole);
                report.InferredArgumentDefinitionIds.assign(
                    evidenceIt->second.InferredArgumentDefinitionIds.begin(),
                    evidenceIt->second.InferredArgumentDefinitionIds.end());
                if (report.InferredKind.has_value()) {
                    report.bRoleMatchesDeclared = *report.InferredKind == definition.Kind;
                    report.bRoleMismatchesDeclared = *report.InferredKind != definition.Kind;
                }
                report.NormalizedInterpretation =
                    RenderRoleInterpretationV2(inferredRole, report.InferredArgumentDefinitionIds);
            } else {
                report.NormalizedInterpretation =
                    RenderRoleInterpretationV2(DefinitionKindToSemanticRoleV2(definition.Kind), {});
            }

            report.Diagnostics = ConvertValidationDiagnosticsV2(validationMessages);
            if (report.bRoleMismatchesDeclared && report.InferredKind.has_value()) {
                AppendDiagnosticIfMissingV2(
                    report.Diagnostics,
                    FSemanticDiagnosticV2{
                        .Severity = EValidationSeverityV2::Warning,
                        .Code = "declared_inferred_mismatch",
                        .EntityId = definition.DefinitionId,
                        .Context = "Definition",
                        .Message = "Declared role '" + Str(ToString(definition.Kind)) +
                            "' disagrees with inferred role '" + Str(ToString(*report.InferredKind)) + "'."
                    });
            }

            if (const auto assumptionIt = assumptionsByTargetId.find(definition.DefinitionId);
                assumptionIt != assumptionsByTargetId.end()) {
                report.Assumptions = assumptionIt->second;
            }
            return report;
        }

        inline auto BuildRelationReportFromModelV2(const FModelV2 &model,
                                                   const FRelationV2 &relation,
                                                   const Vector<FValidationMessageV2> &validationMessages) -> FRelationSemanticReportV2 {
            FRelationSemanticReportV2 report;
            report.RelationId = relation.RelationId;
            report.CanonicalNotation = MakeCanonicalRelationNotationV2(relation, &model);

            std::map<Str, FSymbolEvidenceV2> evidenceByKey;
            FRelationStructureV2 structure;
            CollectExpressionEvidenceV2(relation.Left, model, evidenceByKey, structure);
            CollectExpressionEvidenceV2(relation.Right, model, evidenceByKey, structure);

            report.InferredClass = ClassifyRelationV2(structure);
            report.NormalizedInterpretation = RenderRelationInterpretationV2(report.InferredClass);
            report.Diagnostics = ConvertValidationDiagnosticsV2(validationMessages);

            for (const auto &[key, evidence] : evidenceByKey) {
                (void) key;
                auto symbol = BuildReferencedSymbolSemanticV2(model, evidence);
                AppendInferenceDiagnosticsForSymbolV2(symbol, relation.RelationId, "Relation", report.Diagnostics);
                report.ReferencedSymbols.push_back(std::move(symbol));
            }

            const auto sourceSignature = MakeRelationSourceSignatureV2(report);
            for (const auto &symbol : report.ReferencedSymbols) {
                if (symbol.InferredKind.has_value()) {
                    FSemanticAssumptionV2 assumption;
                    assumption.AssumptionId = MakeRoleAssumptionIdV2(sourceSignature, symbol);
                    assumption.Kind = EAssumptionKindV2::DefinitionRole;
                    assumption.Category = ToString(*symbol.InferredKind);
                    assumption.TargetId = symbol.ReferenceId;
                    assumption.TargetSymbol = symbol.SymbolText;
                    assumption.SourceId = relation.RelationId;
                    assumption.SourceSignature = sourceSignature;
                    assumption.DeclaredKind = symbol.DeclaredKind;
                    assumption.InferredKind = symbol.InferredKind;
                    assumption.ProposedArgumentDefinitionIds = symbol.InferredArgumentDefinitionIds;
                    assumption.bWouldCreateDefinition = !symbol.bResolved;
                    assumption.bMatchesDeclaredRole = symbol.bDeclaredInferredAgreement;
                    assumption.bMismatchesDeclaredRole = symbol.bDeclaredInferredMismatch;
                    assumption.Detail = "Inferred from relation usage.";
                    assumption.ProposedType = BuildProposedTypeForAssumptionV2(
                        model,
                        DefinitionKindToSemanticRoleV2(*symbol.InferredKind),
                        symbol.InferredArgumentDefinitionIds);
                    ApplyPersistedAssumptionStateV2(model, assumption);
                    if (!(assumption.Status == EAssumptionStatusV2::Dismissed)) {
                        report.Assumptions.push_back(std::move(assumption));
                    }
                }

                if (!symbol.InferredArgumentDefinitionIds.empty() &&
                    symbol.InferredKind.has_value() &&
                    *symbol.InferredKind != EDefinitionKindV2::Coordinate) {
                    FSemanticAssumptionV2 assumption;
                    assumption.AssumptionId = MakeDependencyAssumptionIdV2(sourceSignature, symbol);
                    assumption.Kind = EAssumptionKindV2::DefinitionDependency;
                    assumption.Category = "DependsOn";
                    assumption.TargetId = symbol.ReferenceId;
                    assumption.TargetSymbol = symbol.SymbolText;
                    assumption.SourceId = relation.RelationId;
                    assumption.SourceSignature = sourceSignature;
                    assumption.DeclaredKind = symbol.DeclaredKind;
                    assumption.InferredKind = symbol.InferredKind;
                    assumption.ProposedArgumentDefinitionIds = symbol.InferredArgumentDefinitionIds;
                    assumption.Detail = "Inferred dependency on coordinate arguments from derivative structure.";
                    ApplyPersistedAssumptionStateV2(model, assumption);
                    if (!(assumption.Status == EAssumptionStatusV2::Dismissed)) {
                        report.Assumptions.push_back(std::move(assumption));
                    }
                }
            }

            if (report.InferredClass != ERelationSemanticClassV2::Unknown) {
                FSemanticAssumptionV2 assumption;
                assumption.AssumptionId = MakeRelationClassAssumptionIdV2(sourceSignature, report);
                assumption.Kind = EAssumptionKindV2::RelationClass;
                assumption.Category = ToString(report.InferredClass);
                assumption.TargetId = relation.RelationId;
                assumption.SourceId = relation.RelationId;
                assumption.SourceSignature = sourceSignature;
                assumption.Detail = "Inferred from derivative/operator structure.";
                ApplyPersistedAssumptionStateV2(model, assumption);
                if (!(assumption.Status == EAssumptionStatusV2::Dismissed)) {
                    report.Assumptions.push_back(std::move(assumption));
                }
            }

            return report;
        }

    } // namespace Detail

    inline auto BuildModelSemanticReportV2(const FModelV2 &model) -> FModelSemanticReportV2 {
        FModelSemanticReportV2 report;

        const auto validation = ValidateModelV2(model);
        report.Diagnostics = Detail::ConvertValidationDiagnosticsV2(validation.Messages);

        std::map<Str, Vector<FValidationMessageV2>> validationByEntityId;
        for (const auto &message : validation.Messages) {
            validationByEntityId[message.EntityId].push_back(message);
        }

        std::map<Str, Detail::FSymbolEvidenceV2> globalEvidenceByKey;
        for (const auto &definition : model.Definitions) {
            if (!definition.DeclaredType.has_value()) continue;
            Detail::CollectTypeEvidenceV2(*definition.DeclaredType, globalEvidenceByKey);
        }
        for (const auto &relation : model.Relations) {
            Detail::FRelationStructureV2 structure;
            Detail::CollectExpressionEvidenceV2(relation.Left, model, globalEvidenceByKey, structure);
            Detail::CollectExpressionEvidenceV2(relation.Right, model, globalEvidenceByKey, structure);
        }

        std::map<Str, Vector<Str>> usedByRelationsByDefinitionId;
        std::map<Str, Vector<FSemanticAssumptionV2>> assumptionsByTargetId;

        report.Relations.reserve(model.Relations.size());
        for (const auto &relation : model.Relations) {
            const auto relationValidationIt = validationByEntityId.find(relation.RelationId);
            const auto relationMessages = relationValidationIt == validationByEntityId.end()
                ? Vector<FValidationMessageV2>{}
                : relationValidationIt->second;
            auto relationReport = Detail::BuildRelationReportFromModelV2(model, relation, relationMessages);
            for (const auto &symbol : relationReport.ReferencedSymbols) {
                if (symbol.bResolved && !symbol.ReferenceId.empty()) {
                    Detail::InsertUniqueStringV2(usedByRelationsByDefinitionId[symbol.ReferenceId], relation.RelationId);
                }
            }
            for (const auto &assumption : relationReport.Assumptions) {
                const auto assumptionTargetId = assumption.TargetId.empty()
                    ? assumption.TargetSymbol
                    : assumption.TargetId;
                assumptionsByTargetId[assumptionTargetId].push_back(assumption);
                report.Assumptions.push_back(assumption);
            }
            report.Relations.push_back(std::move(relationReport));
        }

        report.Definitions.reserve(model.Definitions.size());
        for (const auto &definition : model.Definitions) {
            const auto definitionValidationIt = validationByEntityId.find(definition.DefinitionId);
            const auto definitionMessages = definitionValidationIt == validationByEntityId.end()
                ? Vector<FValidationMessageV2>{}
                : definitionValidationIt->second;
            report.Definitions.push_back(Detail::BuildDefinitionReportFromModelV2(
                model,
                definition,
                definitionMessages,
                globalEvidenceByKey,
                assumptionsByTargetId,
                usedByRelationsByDefinitionId));
        }

        return report;
    }

    inline auto MakeEditorBufferForDefinitionV2(const FModelV2 &model,
                                                const Str &definitionId) -> TOptional<FModelEditorBufferV2> {
        const auto *definition = FindDefinitionByIdV2(model, definitionId);
        if (definition == nullptr) return std::nullopt;

        FModelEditorBufferV2 buffer;
        buffer.TargetKind = EModelObjectKindV2::Definition;
        buffer.TargetId = definitionId;
        buffer.CanonicalNotation = MakeCanonicalDefinitionNotationV2(*definition, &model);
        buffer.DraftNotation = buffer.CanonicalNotation;
        return buffer;
    }

    inline auto MakeEditorBufferForRelationV2(const FModelV2 &model,
                                              const Str &relationId) -> TOptional<FModelEditorBufferV2> {
        const auto *relation = FindRelationByIdV2(model, relationId);
        if (relation == nullptr) return std::nullopt;

        FModelEditorBufferV2 buffer;
        buffer.TargetKind = EModelObjectKindV2::Relation;
        buffer.TargetId = relationId;
        buffer.CanonicalNotation = MakeCanonicalRelationNotationV2(*relation, &model);
        buffer.DraftNotation = buffer.CanonicalNotation;
        return buffer;
    }

    inline auto SetEditorBufferDraftV2(FModelEditorBufferV2 &buffer,
                                       Str draftNotation) -> void {
        buffer.DraftNotation = std::move(draftNotation);
        buffer.bDraftDirty = buffer.DraftNotation != buffer.CanonicalNotation;
        buffer.bPreviewCurrent = false;
    }

    inline auto CollectDefinitionPreviewReferencesV2(const FModelV2 &model,
                                                     const FDefinitionV2 &definition) -> Vector<FReferencedSymbolSemanticV2> {
        Vector<FReferencedSymbolSemanticV2> symbols;
        if (!definition.DeclaredType.has_value()) return symbols;

        std::map<Str, Detail::FSymbolEvidenceV2> evidenceByKey;
        Detail::CollectTypeEvidenceV2(*definition.DeclaredType, evidenceByKey);
        symbols.reserve(evidenceByKey.size());
        for (const auto &[key, evidence] : evidenceByKey) {
            (void) key;
            symbols.push_back(Detail::BuildReferencedSymbolSemanticV2(model, evidence));
        }
        return symbols;
    }

    inline auto PreviewDefinitionEditV2(const FModelV2 &model,
                                        const FDefinitionV2 &selectedDefinition,
                                        const Str &draftNotation) -> FDefinitionDraftPreviewV2 {
        FDefinitionDraftPreviewV2 preview;
        preview.DefinitionId = selectedDefinition.DefinitionId;
        preview.CanonicalNotation = MakeCanonicalDefinitionNotationV2(selectedDefinition, &model);
        preview.DraftNotation = draftNotation;

        const auto context = FNotationContextV2::FromModel(model);
        const auto parsed = ParseDefinitionNotationV2(draftNotation, &context);
        if (!parsed.IsOk()) {
            preview.ParseError = parsed.Error;
            preview.Diagnostics.push_back(FSemanticDiagnosticV2{
                .Severity = EValidationSeverityV2::Error,
                .Code = "parse_error",
                .EntityId = selectedDefinition.DefinitionId,
                .Context = "Definition",
                .Message = parsed.Error.Message
            });
            preview.bHasBlockingErrors = true;
            return preview;
        }

        preview.bParseOk = true;

        auto proposedDefinition = selectedDefinition;
        proposedDefinition.Symbol = parsed.Value->CanonicalSymbol;
        proposedDefinition.PreferredNotation = parsed.Value->SourceSymbolText;
        proposedDefinition.SourceText = draftNotation;
        proposedDefinition.DeclaredType.reset();
        if (parsed.Value->bMembership && parsed.Value->MembershipSpace.has_value()) {
            proposedDefinition.DeclaredType = MakeSpaceTypeV2(*parsed.Value->MembershipSpace);
        } else if (parsed.Value->DeclaredType.has_value()) {
            proposedDefinition.DeclaredType = *parsed.Value->DeclaredType;
        }

        preview.ProposedDefinition = proposedDefinition;
        preview.NormalizedProjection = RenderDialectDefinitionV2(proposedDefinition, &model);

        auto previewModel = model;
        if (auto *definition = FindDefinitionByIdV2(previewModel, selectedDefinition.DefinitionId); definition != nullptr) {
            *definition = proposedDefinition;
        }

        const auto semanticReport = BuildModelSemanticReportV2(previewModel);
        if (const auto it = std::find_if(
                semanticReport.Definitions.begin(),
                semanticReport.Definitions.end(),
                [&](const auto &report) { return report.DefinitionId == selectedDefinition.DefinitionId; });
            it != semanticReport.Definitions.end()) {
            preview.NormalizedInterpretation = it->NormalizedInterpretation;
            preview.InferredKind = it->InferredKind;
            preview.Assumptions = it->Assumptions;
            preview.Diagnostics = it->Diagnostics;
        }

        preview.ReferencedSymbols = CollectDefinitionPreviewReferencesV2(previewModel, proposedDefinition);
        for (const auto &symbol : preview.ReferencedSymbols) {
            Detail::AppendInferenceDiagnosticsForSymbolV2(
                symbol,
                selectedDefinition.DefinitionId,
                "Definition",
                preview.Diagnostics);
        }

        preview.bHasBlockingErrors = std::any_of(
            semanticReport.Diagnostics.begin(),
            semanticReport.Diagnostics.end(),
            [](const auto &diagnostic) { return diagnostic.Severity == EValidationSeverityV2::Error; });
        preview.bCanApply = preview.bParseOk && !preview.bHasBlockingErrors;
        if (preview.NormalizedInterpretation.empty()) {
            preview.NormalizedInterpretation = preview.NormalizedProjection;
        }
        return preview;
    }

    inline auto PreviewRelationEditV2(const FModelV2 &model,
                                      const FRelationV2 &selectedRelation,
                                      const Str &draftNotation) -> FRelationDraftPreviewV2 {
        FRelationDraftPreviewV2 preview;
        preview.RelationId = selectedRelation.RelationId;
        preview.CanonicalNotation = MakeCanonicalRelationNotationV2(selectedRelation, &model);
        preview.DraftNotation = draftNotation;

        const auto context = FNotationContextV2::FromModel(model);
        const auto parsed = ParseRelationNotationV2(
            selectedRelation.RelationId,
            selectedRelation.Kind,
            draftNotation,
            &context);
        if (!parsed.IsOk()) {
            preview.ParseError = parsed.Error;
            preview.Diagnostics.push_back(FSemanticDiagnosticV2{
                .Severity = EValidationSeverityV2::Error,
                .Code = "parse_error",
                .EntityId = selectedRelation.RelationId,
                .Context = "Relation",
                .Message = parsed.Error.Message
            });
            preview.bHasBlockingErrors = true;
            return preview;
        }

        preview.bParseOk = true;

        auto proposedRelation = *parsed.Value;
        proposedRelation.Name = selectedRelation.Name;
        proposedRelation.Description = selectedRelation.Description;
        proposedRelation.Tags = selectedRelation.Tags;
        proposedRelation.Metadata = selectedRelation.Metadata;
        preview.ProposedRelation = proposedRelation;

        auto previewModel = model;
        if (auto *relation = FindRelationByIdV2(previewModel, selectedRelation.RelationId); relation != nullptr) {
            *relation = proposedRelation;
        }

        const auto semanticReport = BuildModelSemanticReportV2(previewModel);
        if (const auto it = std::find_if(
                semanticReport.Relations.begin(),
                semanticReport.Relations.end(),
                [&](const auto &report) { return report.RelationId == selectedRelation.RelationId; });
            it != semanticReport.Relations.end()) {
            preview.NormalizedProjection = RenderDialectRelationV2(proposedRelation, &previewModel);
            preview.NormalizedInterpretation = it->NormalizedInterpretation;
            preview.InferredClass = it->InferredClass;
            preview.ReferencedSymbols = it->ReferencedSymbols;
            preview.Diagnostics = it->Diagnostics;
            preview.Assumptions = it->Assumptions;
        }

        preview.bHasBlockingErrors = std::any_of(
            semanticReport.Diagnostics.begin(),
            semanticReport.Diagnostics.end(),
            [](const auto &diagnostic) { return diagnostic.Severity == EValidationSeverityV2::Error; });
        preview.bCanApply = preview.bParseOk && !preview.bHasBlockingErrors;
        if (preview.NormalizedProjection.empty()) {
            preview.NormalizedProjection = RenderDialectRelationV2(proposedRelation, &previewModel);
        }
        if (preview.NormalizedInterpretation.empty()) {
            preview.NormalizedInterpretation = preview.NormalizedProjection;
        }
        return preview;
    }

    inline auto ParseEditorBufferPreviewV2(const FModelV2 &model,
                                           FModelEditorBufferV2 &buffer) -> bool {
        buffer.DefinitionPreview.reset();
        buffer.RelationPreview.reset();
        buffer.bPreviewCurrent = true;

        if (buffer.TargetKind == EModelObjectKindV2::Definition) {
            const auto *definition = FindDefinitionByIdV2(model, buffer.TargetId);
            if (definition == nullptr) return false;
            buffer.DefinitionPreview = PreviewDefinitionEditV2(model, *definition, buffer.DraftNotation);
            return buffer.DefinitionPreview->bParseOk;
        }

        if (buffer.TargetKind == EModelObjectKindV2::Relation) {
            const auto *relation = FindRelationByIdV2(model, buffer.TargetId);
            if (relation == nullptr) return false;
            buffer.RelationPreview = PreviewRelationEditV2(model, *relation, buffer.DraftNotation);
            return buffer.RelationPreview->bParseOk;
        }

        return false;
    }

    inline auto RevertEditorBufferV2(const FModelV2 &model,
                                     FModelEditorBufferV2 &buffer) -> bool {
        if (buffer.TargetKind == EModelObjectKindV2::Definition) {
            const auto *definition = FindDefinitionByIdV2(model, buffer.TargetId);
            if (definition == nullptr) return false;
            buffer.CanonicalNotation = MakeCanonicalDefinitionNotationV2(*definition, &model);
        } else if (buffer.TargetKind == EModelObjectKindV2::Relation) {
            const auto *relation = FindRelationByIdV2(model, buffer.TargetId);
            if (relation == nullptr) return false;
            buffer.CanonicalNotation = MakeCanonicalRelationNotationV2(*relation, &model);
        } else {
            return false;
        }

        buffer.DraftNotation = buffer.CanonicalNotation;
        buffer.bDraftDirty = false;
        buffer.bPreviewCurrent = false;
        buffer.DefinitionPreview.reset();
        buffer.RelationPreview.reset();
        return true;
    }

    inline auto ApplyModelChangeRecordV2(FModelV2 &model,
                                         FModelChangeRecordV2 changeRecord) -> void {
        if (changeRecord.ChangeId.empty()) {
            changeRecord.ChangeId = changeRecord.ObjectId + ":" +
                StableHashStringV2(changeRecord.PreviousCanonicalNotation + "->" + changeRecord.NewCanonicalNotation);
        }
        model.ChangeLog.push_back(std::move(changeRecord));
    }

    inline auto ApplyEditorBufferV2(FModelV2 &model,
                                    FModelEditorBufferV2 &buffer,
                                    const EModelChangeOriginV2 origin = EModelChangeOriginV2::DirectEdit,
                                    FModelChangeRecordV2 *pOutChangeRecord = nullptr) -> bool {
        if (!buffer.bPreviewCurrent) {
            if (!ParseEditorBufferPreviewV2(model, buffer)) return false;
        }

        if (buffer.TargetKind == EModelObjectKindV2::Definition) {
            if (!buffer.DefinitionPreview.has_value() || !buffer.DefinitionPreview->bCanApply || !buffer.DefinitionPreview->ProposedDefinition.has_value()) {
                return false;
            }

            auto *definition = FindDefinitionByIdV2(model, buffer.TargetId);
            if (definition == nullptr) return false;

            FModelChangeRecordV2 changeRecord;
            changeRecord.ObjectKind = EModelObjectKindV2::Definition;
            changeRecord.ObjectId = definition->DefinitionId;
            changeRecord.PreviousCanonicalNotation = MakeCanonicalDefinitionNotationV2(*definition, &model);
            changeRecord.NewCanonicalNotation = buffer.DraftNotation;
            changeRecord.Origin = origin;
            *definition = *buffer.DefinitionPreview->ProposedDefinition;
            ApplyModelChangeRecordV2(model, changeRecord);
            if (pOutChangeRecord != nullptr) *pOutChangeRecord = model.ChangeLog.back();
        } else if (buffer.TargetKind == EModelObjectKindV2::Relation) {
            if (!buffer.RelationPreview.has_value() || !buffer.RelationPreview->bCanApply || !buffer.RelationPreview->ProposedRelation.has_value()) {
                return false;
            }

            auto *relation = FindRelationByIdV2(model, buffer.TargetId);
            if (relation == nullptr) return false;

            FModelChangeRecordV2 changeRecord;
            changeRecord.ObjectKind = EModelObjectKindV2::Relation;
            changeRecord.ObjectId = relation->RelationId;
            changeRecord.PreviousCanonicalNotation = MakeCanonicalRelationNotationV2(*relation, &model);
            changeRecord.NewCanonicalNotation = buffer.DraftNotation;
            changeRecord.Origin = origin;
            *relation = *buffer.RelationPreview->ProposedRelation;
            ApplyModelChangeRecordV2(model, changeRecord);
            if (pOutChangeRecord != nullptr) *pOutChangeRecord = model.ChangeLog.back();
        } else {
            return false;
        }

        return RevertEditorBufferV2(model, buffer);
    }

    inline auto SetAssumptionStatusV2(FModelV2 &model,
                                      const Str &assumptionId,
                                      const EAssumptionStatusV2 status,
                                      const Str &materializedDefinitionId = {}) -> bool {
        if (assumptionId.empty()) return false;
        auto &state = Detail::EnsureAssumptionStateV2(model, assumptionId);
        state.Status = status;
        if (!materializedDefinitionId.empty()) state.MaterializedDefinitionId = materializedDefinitionId;
        return true;
    }

    inline auto AcceptAssumptionV2(FModelV2 &model,
                                   const FSemanticAssumptionV2 &assumption,
                                   FModelChangeRecordV2 *pOutChangeRecord = nullptr) -> bool {
        if (assumption.AssumptionId.empty()) return false;

        if (assumption.bWouldCreateDefinition && assumption.InferredKind.has_value()) {
            if (const auto proposedKind = assumption.InferredKind; proposedKind.has_value()) {
                FDefinitionV2 definition;
                definition.DefinitionId = Detail::BuildDefinitionIdSuggestionV2(model, *proposedKind, assumption.TargetSymbol);
                definition.Symbol = NormalizeSymbolAliasV2(assumption.TargetSymbol);
                definition.PreferredNotation = assumption.TargetSymbol;
                definition.DisplayName = assumption.TargetSymbol;
                definition.Kind = *proposedKind;
                definition.SourceText = assumption.TargetSymbol;
                definition.ArgumentDefinitionIds = assumption.ProposedArgumentDefinitionIds;
                if (assumption.ProposedType.has_value()) definition.DeclaredType = *assumption.ProposedType;
                definition.Metadata["accepted_assumption_id"] = assumption.AssumptionId;
                model.Definitions.push_back(definition);

                SetAssumptionStatusV2(model, assumption.AssumptionId, EAssumptionStatusV2::Accepted, definition.DefinitionId);

                FModelChangeRecordV2 changeRecord;
                changeRecord.ObjectKind = EModelObjectKindV2::Definition;
                changeRecord.ObjectId = definition.DefinitionId;
                changeRecord.PreviousCanonicalNotation = {};
                changeRecord.NewCanonicalNotation = RenderDialectDefinitionV2(definition, &model);
                changeRecord.Origin = EModelChangeOriginV2::AcceptedInference;
                changeRecord.SourceId = assumption.AssumptionId;
                ApplyModelChangeRecordV2(model, changeRecord);
                if (pOutChangeRecord != nullptr) *pOutChangeRecord = model.ChangeLog.back();
                return true;
            }
        }

        SetAssumptionStatusV2(model, assumption.AssumptionId, EAssumptionStatusV2::Accepted, assumption.MaterializedDefinitionId);

        FModelChangeRecordV2 changeRecord;
        changeRecord.ObjectKind = EModelObjectKindV2::Assumption;
        changeRecord.ObjectId = assumption.AssumptionId;
        changeRecord.PreviousCanonicalNotation = ToString(EAssumptionStatusV2::Implicit);
        changeRecord.NewCanonicalNotation = ToString(EAssumptionStatusV2::Accepted);
        changeRecord.Origin = EModelChangeOriginV2::AcceptedInference;
        changeRecord.SourceId = assumption.SourceId;
        ApplyModelChangeRecordV2(model, changeRecord);
        if (pOutChangeRecord != nullptr) *pOutChangeRecord = model.ChangeLog.back();
        return true;
    }

} // namespace Slab::Core::Model::V2

#endif // STUDIOSLAB_MODEL_AUTHORING_V2_H
