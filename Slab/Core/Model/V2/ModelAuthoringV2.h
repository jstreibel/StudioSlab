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
        ESemanticOriginV2 Origin = ESemanticOriginV2::Unresolved;
        Str OriginDetail;
        Str SourcePresetId;
        TOptional<EBaseVocabularyEntryKindV2> VocabularyKind;
        Str SemanticRoleSummary;
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
        Vector<FReferencedSymbolSemanticV2> ReferencedSymbols;
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

    enum class ESemanticObjectKindV2 : unsigned char {
        Definition,
        Relation,
        VocabularyEntry,
        Assumption
    };

    enum class ESemanticHealthV2 : unsigned char {
        Ok,
        Deferred,
        Warning,
        Error
    };

    struct FSemanticObjectRefV2 {
        ESemanticObjectKindV2 Kind = ESemanticObjectKindV2::Definition;
        Str ObjectId;

        [[nodiscard]] auto IsValid() const -> bool {
            return !ObjectId.empty();
        }
    };

    struct FSemanticNavigationLinkV2 {
        FSemanticObjectRefV2 Target;
        Str Label;
        Str Detail;
        bool bAmbient = false;
        bool bReadonly = false;
        bool bConflict = false;
        bool bOverride = false;
    };

    struct FSemanticDiagnosticNavigationV2 {
        FSemanticDiagnosticV2 Diagnostic;
        TOptional<FSemanticObjectRefV2> NavigateTo;
    };

    struct FSemanticObjectOverviewV2 {
        FSemanticObjectRefV2 Ref;
        Str DisplayLabel;
        Str CanonicalNotation;
        Str KindLabel;
        Str CategoryLabel;
        Str OriginLabel;
        Str OriginDetail;
        Str StatusLabel;
        Str Description;
        bool bReadonly = false;
        bool bAmbient = false;
        bool bConflict = false;
        bool bLocalOverride = false;
        TOptional<EDefinitionKindV2> DeclaredKind;
        TOptional<EDefinitionKindV2> InferredKind;
        TOptional<ERelationSemanticClassV2> InferredRelationClass;
        TOptional<EAssumptionStatusV2> AssumptionStatus;
        Vector<FSemanticNavigationLinkV2> SourceLinks;
        Vector<FSemanticNavigationLinkV2> TargetLinks;
        Vector<FSemanticNavigationLinkV2> DependsOn;
        Vector<FSemanticNavigationLinkV2> AmbientDependencies;
        Vector<FSemanticNavigationLinkV2> UsedBy;
        Vector<FSemanticNavigationLinkV2> RelatedAssumptions;
        Vector<FSemanticNavigationLinkV2> LocalOverrides;
        Vector<FSemanticDiagnosticNavigationV2> Diagnostics;
    };

    struct FModelSemanticClassificationV2 {
        Str ModelClass;
        Str Character;
        StrVector Traits;
        bool bInferred = true;
    };

    struct FModelSemanticStatusSummaryV2 {
        ESemanticHealthV2 ParseHealth = ESemanticHealthV2::Ok;
        ESemanticHealthV2 SemanticHealth = ESemanticHealthV2::Ok;
        std::size_t ValidationErrorCount = 0;
        std::size_t ValidationWarningCount = 0;
        std::size_t UnresolvedSymbolCount = 0;
        std::size_t PendingAssumptionCount = 0;
        std::size_t AcceptedAssumptionCount = 0;
        std::size_t DismissedAssumptionCount = 0;
        std::size_t OverrideCount = 0;
        std::size_t SpecializationCount = 0;
        std::size_t DeferredCount = 0;
        Str ActiveBaseVocabularyId;
        Str ActiveBaseVocabularyName;
        FModelSemanticClassificationV2 Classification;
        Vector<FSemanticNavigationLinkV2> CanonicalStateVariables;
        Vector<FSemanticNavigationLinkV2> Parameters;
        Vector<FSemanticNavigationLinkV2> Fields;
        Vector<FSemanticNavigationLinkV2> Operators;
        Vector<FSemanticNavigationLinkV2> Observables;
    };

    struct FModelSemanticOverviewV2 {
        Vector<FResolvedBaseVocabularyEntryV2> VocabularyEntries;
        FValidationResultV2 Validation;
        FModelSemanticReportV2 Report;
        FModelSemanticStatusSummaryV2 Status;
        std::map<Str, FSemanticObjectOverviewV2> ObjectsByKey;

        [[nodiscard]] auto FindObject(const FSemanticObjectRefV2 &ref) const -> const FSemanticObjectOverviewV2 * {
            Str prefix = "Definition";
            switch (ref.Kind) {
                case ESemanticObjectKindV2::Definition: prefix = "Definition"; break;
                case ESemanticObjectKindV2::Relation: prefix = "Relation"; break;
                case ESemanticObjectKindV2::VocabularyEntry: prefix = "VocabularyEntry"; break;
                case ESemanticObjectKindV2::Assumption: prefix = "Assumption"; break;
            }
            const auto it = ObjectsByKey.find(prefix + Str("::") + ref.ObjectId);
            if (it == ObjectsByKey.end()) return nullptr;
            return &it->second;
        }
    };

    struct FSemanticSelectionContextV2 {
        FSemanticObjectRefV2 Selected;
        std::set<Str> DependencyKeys;
        std::set<Str> AmbientDependencyKeys;
        std::set<Str> UsedByKeys;
        std::set<Str> RelatedAssumptionKeys;
        std::set<Str> SourceKeys;
        std::set<Str> TargetKeys;
        std::set<Str> OverrideKeys;
        std::set<Str> ConflictKeys;
    };

    enum class ESemanticDeltaKindV2 : unsigned char {
        Added,
        Removed,
        Changed
    };

    struct FSemanticNavigationDeltaV2 {
        ESemanticDeltaKindV2 Kind = ESemanticDeltaKindV2::Added;
        FSemanticNavigationLinkV2 Link;
        Str PreviousDetail;
        Str NewDetail;
    };

    struct FReferencedSymbolDeltaV2 {
        ESemanticDeltaKindV2 Kind = ESemanticDeltaKindV2::Added;
        FReferencedSymbolSemanticV2 Symbol;
        bool bResolutionChanged = false;
        bool bOriginChanged = false;
        bool bDeclaredKindChanged = false;
        bool bInferredKindChanged = false;
        bool bArgumentDependenciesChanged = false;
        bool bCanonicalResolved = false;
        ESemanticOriginV2 CanonicalOrigin = ESemanticOriginV2::Unresolved;
        TOptional<EDefinitionKindV2> CanonicalDeclaredKind;
        TOptional<EDefinitionKindV2> CanonicalInferredKind;
        Vector<Str> CanonicalArgumentDefinitionIds;
    };

    struct FSemanticAssumptionDeltaV2 {
        ESemanticDeltaKindV2 Kind = ESemanticDeltaKindV2::Added;
        FSemanticAssumptionV2 Assumption;
        EAssumptionStatusV2 CanonicalStatus = EAssumptionStatusV2::Implicit;
        Str CanonicalMaterializedDefinitionId;
        bool bStatusChanged = false;
        bool bMaterializationChanged = false;
    };

    struct FSemanticDiagnosticDeltaV2 {
        ESemanticDeltaKindV2 Kind = ESemanticDeltaKindV2::Added;
        FSemanticDiagnosticV2 Diagnostic;
    };

    struct FModelSemanticDraftDeltaV2 {
        bool bAvailable = false;
        FModelSemanticStatusSummaryV2 CanonicalStatus;
        FModelSemanticStatusSummaryV2 DraftStatus;
        Str CanonicalInterpretation;
        Str DraftInterpretation;
        TOptional<EDefinitionKindV2> CanonicalInferredKind;
        TOptional<EDefinitionKindV2> DraftInferredKind;
        TOptional<ERelationSemanticClassV2> CanonicalRelationClass;
        TOptional<ERelationSemanticClassV2> DraftRelationClass;
        bool bCanonicalConflict = false;
        bool bDraftConflict = false;
        Vector<FReferencedSymbolDeltaV2> ReferencedSymbolChanges;
        Vector<FSemanticNavigationDeltaV2> DependencyChanges;
        Vector<FSemanticNavigationDeltaV2> AmbientDependencyChanges;
        Vector<FSemanticNavigationDeltaV2> UsedByChanges;
        Vector<FSemanticNavigationDeltaV2> AssumptionLinkChanges;
        Vector<FSemanticNavigationDeltaV2> OverrideChanges;
        Vector<FSemanticAssumptionDeltaV2> AssumptionChanges;
        Vector<FSemanticDiagnosticDeltaV2> DiagnosticChanges;
    };

    enum class EAssumptionLifecyclePhaseV2 : unsigned char {
        Implicit,
        Accepted,
        Dismissed,
        Materialized
    };

    struct FAssumptionMaterializationPreviewV2 {
        bool bAvailable = false;
        bool bWouldCreateDefinition = false;
        bool bAlreadyMaterialized = false;
        EAssumptionLifecyclePhaseV2 LifecyclePhase = EAssumptionLifecyclePhaseV2::Implicit;
        Str LifecycleLabel;
        Str OutcomeLabel;
        Str SuggestedDefinitionId;
        Str ProposedNotation;
        TOptional<FTypeExprV2> ProposedType;
        TOptional<FDefinitionV2> ProposedDefinition;
        TOptional<FSemanticObjectRefV2> MaterializedObjectRef;
        Vector<FSemanticNavigationLinkV2> ProposedDependencies;
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
        TOptional<FModelSemanticOverviewV2> SemanticOverview;
        TOptional<FModelSemanticDraftDeltaV2> SemanticDelta;
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
        TOptional<FModelSemanticOverviewV2> SemanticOverview;
        TOptional<FModelSemanticDraftDeltaV2> SemanticDelta;
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

    inline auto ToString(const ESemanticDeltaKindV2 value) -> const char * {
        switch (value) {
            case ESemanticDeltaKindV2::Added: return "Added";
            case ESemanticDeltaKindV2::Removed: return "Removed";
            case ESemanticDeltaKindV2::Changed: return "Changed";
        }

        return "Changed";
    }

    inline auto ToString(const EAssumptionLifecyclePhaseV2 value) -> const char * {
        switch (value) {
            case EAssumptionLifecyclePhaseV2::Implicit: return "Implicit";
            case EAssumptionLifecyclePhaseV2::Accepted: return "Accepted";
            case EAssumptionLifecyclePhaseV2::Dismissed: return "Dismissed";
            case EAssumptionLifecyclePhaseV2::Materialized: return "Materialized";
        }

        return "Implicit";
    }

    inline auto ToString(const ESemanticObjectKindV2 value) -> const char * {
        switch (value) {
            case ESemanticObjectKindV2::Definition: return "Definition";
            case ESemanticObjectKindV2::Relation: return "Relation";
            case ESemanticObjectKindV2::VocabularyEntry: return "VocabularyEntry";
            case ESemanticObjectKindV2::Assumption: return "Assumption";
        }

        return "Unknown";
    }

    inline auto ToString(const ESemanticHealthV2 value) -> const char * {
        switch (value) {
            case ESemanticHealthV2::Ok: return "Ok";
            case ESemanticHealthV2::Deferred: return "Deferred";
            case ESemanticHealthV2::Warning: return "Warning";
            case ESemanticHealthV2::Error: return "Error";
        }

        return "Unknown";
    }

    inline auto MakeDefinitionObjectRefV2(const Str &definitionId) -> FSemanticObjectRefV2 {
        return FSemanticObjectRefV2{ESemanticObjectKindV2::Definition, definitionId};
    }

    inline auto MakeRelationObjectRefV2(const Str &relationId) -> FSemanticObjectRefV2 {
        return FSemanticObjectRefV2{ESemanticObjectKindV2::Relation, relationId};
    }

    inline auto MakeVocabularyObjectRefV2(const Str &entryId) -> FSemanticObjectRefV2 {
        return FSemanticObjectRefV2{ESemanticObjectKindV2::VocabularyEntry, entryId};
    }

    inline auto MakeAssumptionObjectRefV2(const Str &assumptionId) -> FSemanticObjectRefV2 {
        return FSemanticObjectRefV2{ESemanticObjectKindV2::Assumption, assumptionId};
    }

    inline auto MakeSemanticObjectKeyV2(const FSemanticObjectRefV2 &ref) -> Str {
        return Str(ToString(ref.Kind)) + "::" + ref.ObjectId;
    }

    inline auto AreSemanticObjectRefsEqualV2(const FSemanticObjectRefV2 &lhs,
                                             const FSemanticObjectRefV2 &rhs) -> bool {
        return lhs.Kind == rhs.Kind && lhs.ObjectId == rhs.ObjectId;
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

    inline auto BuildAssumptionMaterializationPreviewV2(const FModelV2 &model,
                                                        const FSemanticAssumptionV2 &assumption,
                                                        const FModelSemanticOverviewV2 *pOverview = nullptr)
        -> FAssumptionMaterializationPreviewV2;

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
            bool bSeenAsSpace = false;
            bool bSeenAsNotationConvention = false;
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

        inline auto RegisterAmbientAliasEvidenceV2(const FModelV2 &model,
                                                   std::map<Str, FSymbolEvidenceV2> &evidenceByKey,
                                                   const Str &alias,
                                                   const char *usageLabel,
                                                   const bool bAsSpace = false,
                                                   const bool bAsNotationConvention = false) -> void {
            const auto reference = FindBaseVocabularyReferenceByAliasV2(model, alias)
                .value_or(MakeReferenceV2({}, alias));
            auto &entry = RegisterEvidenceV2(evidenceByKey, reference);
            if (bAsSpace) entry.bSeenAsSpace = true;
            if (bAsNotationConvention) entry.bSeenAsNotationConvention = true;
            entry.UsageLabels.insert(usageLabel);
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
                                          const FModelV2 &model,
                                          std::map<Str, FSymbolEvidenceV2> &evidenceByKey) -> void {
            if (type.Kind == ETypeExprKindV2::Space) {
                if (type.Space.Kind == ESpaceKindV2::RealNumbers) {
                    RegisterAmbientAliasEvidenceV2(model, evidenceByKey, "\\mathbb{R}", "space", true, false);
                } else if (type.Space.Kind == ESpaceKindV2::ComplexNumbers) {
                    RegisterAmbientAliasEvidenceV2(model, evidenceByKey, "\\mathbb{C}", "space", true, false);
                } else {
                    const auto alias = type.Space.Name == "N"
                        ? Str("\\mathbb{N}")
                        : type.Space.Name;
                    RegisterAmbientAliasEvidenceV2(model, evidenceByKey, alias, "space", true, false);
                }
                if (type.Space.Dimension.has_value()) CollectTypeDimensionEvidenceV2(*type.Space.Dimension, evidenceByKey);
                return;
            }

            for (const auto &domainType : type.Domain) {
                CollectTypeEvidenceV2(domainType, model, evidenceByKey);
            }
            if (type.Codomain != nullptr) CollectTypeEvidenceV2(*type.Codomain, model, evidenceByKey);
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
                        if (order == 1) {
                            RegisterAmbientAliasEvidenceV2(model, evidenceByKey, "\\dot", "notation-convention", false, true);
                        } else if (order == 2) {
                            RegisterAmbientAliasEvidenceV2(model, evidenceByKey, "\\ddot", "notation-convention", false, true);
                        }
                    } else {
                        structure.MaxPartialDerivativeOrder = std::max(structure.MaxPartialDerivativeOrder, order);
                        structure.bUsesFieldLikeUsage = true;
                        RegisterAmbientAliasEvidenceV2(model, evidenceByKey, "\\partial", "notation-convention", false, true);
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
            if (evidence.bSeenAsSpace || evidence.bSeenAsNotationConvention) {
                return EDefinitionSemanticRoleV2::Unknown;
            }
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
                const auto resolved = ResolveSemanticEntryV2(model, MakeReferenceV2(evidence.ReferenceId, evidence.SymbolText));
                symbol.Origin = resolved.Origin;
                if (resolved.Definition != nullptr) {
                    symbol.DeclaredKind = resolved.Definition->Kind;
                    symbol.OriginDetail = resolved.Definition->DefinitionId;
                } else if (resolved.VocabularyEntry.has_value()) {
                    symbol.DeclaredKind = resolved.VocabularyEntry->DefinitionKind;
                    symbol.OriginDetail = resolved.VocabularyEntry->SourcePresetId;
                    symbol.SourcePresetId = resolved.VocabularyEntry->SourcePresetId;
                    symbol.VocabularyKind = resolved.VocabularyEntry->Kind;
                    symbol.SemanticRoleSummary = resolved.VocabularyEntry->SemanticRoleSummary;
                }

                if (symbol.InferredKind.has_value() && symbol.DeclaredKind.has_value()) {
                    symbol.bDeclaredInferredAgreement = *symbol.InferredKind == *symbol.DeclaredKind;
                    symbol.bDeclaredInferredMismatch = *symbol.InferredKind != *symbol.DeclaredKind;
                }
            } else {
                symbol.Origin = ESemanticOriginV2::Unresolved;
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

        inline auto BuildDefinitionFromAssumptionV2(const FModelV2 &model,
                                                    const FSemanticAssumptionV2 &assumption) -> TOptional<FDefinitionV2> {
            if (!(assumption.bWouldCreateDefinition && assumption.InferredKind.has_value())) return std::nullopt;

            FDefinitionV2 definition;
            definition.DefinitionId = BuildDefinitionIdSuggestionV2(model, *assumption.InferredKind, assumption.TargetSymbol);
            definition.Symbol = NormalizeSymbolAliasV2(assumption.TargetSymbol);
            definition.PreferredNotation = assumption.TargetSymbol.empty() ? definition.Symbol : assumption.TargetSymbol;
            definition.DisplayName = definition.PreferredNotation.empty() ? definition.DefinitionId : definition.PreferredNotation;
            definition.Kind = *assumption.InferredKind;
            definition.SourceText = definition.PreferredNotation.empty() ? definition.Symbol : definition.PreferredNotation;
            definition.ArgumentDefinitionIds = assumption.ProposedArgumentDefinitionIds;
            if (assumption.ProposedType.has_value()) definition.DeclaredType = *assumption.ProposedType;
            definition.Metadata["accepted_assumption_id"] = assumption.AssumptionId;
            return definition;
        }

        inline auto EnsureAssumptionStateV2(FModelV2 &model, const Str &assumptionId) -> FAssumptionStateV2 & {
            if (auto *existing = FindAssumptionStateByIdV2(model, assumptionId); existing != nullptr) return *existing;
            model.AssumptionStates.push_back(FAssumptionStateV2{.AssumptionId = assumptionId});
            return model.AssumptionStates.back();
        }

        inline auto BuildReferencedSymbolKeyV2(const FReferencedSymbolSemanticV2 &symbol) -> Str {
            if (!symbol.ReferenceId.empty()) return "ref:" + symbol.ReferenceId;
            return "sym:" + NormalizeSymbolAliasV2(symbol.SymbolText);
        }

        inline auto BuildDiagnosticKeyV2(const FSemanticDiagnosticV2 &diagnostic) -> Str {
            return diagnostic.Code + "|" + diagnostic.EntityId + "|" + diagnostic.Context + "|" + diagnostic.Message;
        }

        inline auto BuildNavigationLinkKeyV2(const FSemanticNavigationLinkV2 &link) -> Str {
            return MakeSemanticObjectKeyV2(link.Target);
        }

        template<typename TValue>
        inline auto OptionalValuesEqualV2(const TOptional<TValue> &lhs,
                                          const TOptional<TValue> &rhs) -> bool {
            if (lhs.has_value() != rhs.has_value()) return false;
            if (!lhs.has_value()) return true;
            return *lhs == *rhs;
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

            report.Diagnostics = ConvertValidationDiagnosticsV2(validationMessages);

            if (definition.DeclaredType.has_value()) {
                std::map<Str, FSymbolEvidenceV2> typeEvidenceByKey;
                CollectTypeEvidenceV2(*definition.DeclaredType, model, typeEvidenceByKey);
                report.ReferencedSymbols.reserve(typeEvidenceByKey.size());
                for (const auto &[key, evidence] : typeEvidenceByKey) {
                    (void) key;
                    auto symbol = BuildReferencedSymbolSemanticV2(model, evidence);
                    AppendInferenceDiagnosticsForSymbolV2(symbol, definition.DefinitionId, "Definition", report.Diagnostics);
                    report.ReferencedSymbols.push_back(std::move(symbol));
                }
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
                if (symbol.InferredKind.has_value() && symbol.Origin != ESemanticOriginV2::BaseVocabulary) {
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
                    symbol.Origin != ESemanticOriginV2::BaseVocabulary &&
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
            Detail::CollectTypeEvidenceV2(*definition.DeclaredType, model, globalEvidenceByKey);
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

    namespace Detail {

        inline auto FindDefinitionSemanticReportV2(const FModelSemanticReportV2 &report,
                                                   const Str &definitionId) -> const FDefinitionSemanticReportV2 * {
            const auto it = std::find_if(report.Definitions.begin(), report.Definitions.end(), [&](const auto &entry) {
                return entry.DefinitionId == definitionId;
            });
            if (it == report.Definitions.end()) return nullptr;
            return &(*it);
        }

        inline auto FindRelationSemanticReportV2(const FModelSemanticReportV2 &report,
                                                 const Str &relationId) -> const FRelationSemanticReportV2 * {
            const auto it = std::find_if(report.Relations.begin(), report.Relations.end(), [&](const auto &entry) {
                return entry.RelationId == relationId;
            });
            if (it == report.Relations.end()) return nullptr;
            return &(*it);
        }

        inline auto FindSemanticAssumptionV2(const FModelSemanticReportV2 &report,
                                             const Str &assumptionId) -> const FSemanticAssumptionV2 * {
            const auto it = std::find_if(report.Assumptions.begin(), report.Assumptions.end(), [&](const auto &entry) {
                return entry.AssumptionId == assumptionId;
            });
            if (it == report.Assumptions.end()) return nullptr;
            return &(*it);
        }

        inline auto FindResolvedVocabularyEntryByIdV2(const Vector<FResolvedBaseVocabularyEntryV2> &entries,
                                                      const Str &entryId) -> const FResolvedBaseVocabularyEntryV2 * {
            const auto it = std::find_if(entries.begin(), entries.end(), [&](const auto &entry) {
                return entry.Entry.EntryId == entryId;
            });
            if (it == entries.end()) return nullptr;
            return &(*it);
        }

        inline auto ResolveObjectRefByIdV2(const FModelV2 &model,
                                           const Str &objectId) -> TOptional<FSemanticObjectRefV2> {
            if (objectId.empty()) return std::nullopt;
            if (FindDefinitionByIdV2(model, objectId) != nullptr) return MakeDefinitionObjectRefV2(objectId);
            if (FindRelationByIdV2(model, objectId) != nullptr) return MakeRelationObjectRefV2(objectId);
            if (FindBaseVocabularyEntryByIdV2(model, objectId).has_value()) return MakeVocabularyObjectRefV2(objectId);
            return std::nullopt;
        }

        inline auto ResolveObjectRefForReferencedSymbolV2(const FReferencedSymbolSemanticV2 &symbol)
            -> TOptional<FSemanticObjectRefV2> {
            if (!symbol.bResolved || symbol.ReferenceId.empty()) return std::nullopt;
            if (symbol.Origin == ESemanticOriginV2::BaseVocabulary) return MakeVocabularyObjectRefV2(symbol.ReferenceId);
            if (symbol.Origin == ESemanticOriginV2::LocalDefinition) return MakeDefinitionObjectRefV2(symbol.ReferenceId);
            return std::nullopt;
        }

        inline auto ResolveDraftObjectRefForReferencedSymbolV2(const FModelSemanticOverviewV2 &overview,
                                                               const FReferencedSymbolSemanticV2 &symbol)
            -> TOptional<FSemanticObjectRefV2> {
            if (const auto ref = ResolveObjectRefForReferencedSymbolV2(symbol); ref.has_value()) {
                return ref;
            }

            for (const auto &[key, object] : overview.ObjectsByKey) {
                (void) key;
                if (object.Ref.Kind != ESemanticObjectKindV2::Definition) continue;
                if (object.StatusLabel != "draft only") continue;
                if (!symbol.SymbolText.empty() && object.DisplayLabel == symbol.SymbolText) {
                    return object.Ref;
                }
                if (!symbol.SymbolText.empty() && object.CanonicalNotation.find(symbol.SymbolText) != Str::npos) {
                    return object.Ref;
                }
            }

            for (const auto &assumption : overview.Report.Assumptions) {
                if (!symbol.SymbolText.empty() && assumption.TargetSymbol == symbol.SymbolText) {
                    return MakeAssumptionObjectRefV2(assumption.AssumptionId);
                }
                if (!symbol.ReferenceId.empty() && assumption.TargetId == symbol.ReferenceId) {
                    return MakeAssumptionObjectRefV2(assumption.AssumptionId);
                }
            }

            return std::nullopt;
        }

        inline auto ResolveAssumptionTargetRefV2(const FModelV2 &model,
                                                 const FSemanticAssumptionV2 &assumption)
            -> TOptional<FSemanticObjectRefV2> {
            if (!assumption.MaterializedDefinitionId.empty() &&
                FindDefinitionByIdV2(model, assumption.MaterializedDefinitionId) != nullptr) {
                return MakeDefinitionObjectRefV2(assumption.MaterializedDefinitionId);
            }
            return ResolveObjectRefByIdV2(model, assumption.TargetId);
        }

        inline auto FindObjectOverviewV2(std::map<Str, FSemanticObjectOverviewV2> &objectsByKey,
                                         const FSemanticObjectRefV2 &ref) -> FSemanticObjectOverviewV2 * {
            const auto it = objectsByKey.find(MakeSemanticObjectKeyV2(ref));
            if (it == objectsByKey.end()) return nullptr;
            return &it->second;
        }

        inline auto FindObjectOverviewV2(const std::map<Str, FSemanticObjectOverviewV2> &objectsByKey,
                                         const FSemanticObjectRefV2 &ref) -> const FSemanticObjectOverviewV2 * {
            const auto it = objectsByKey.find(MakeSemanticObjectKeyV2(ref));
            if (it == objectsByKey.end()) return nullptr;
            return &it->second;
        }

        inline auto MakeObjectLabelFallbackV2(const FSemanticObjectRefV2 &ref) -> Str {
            if (!ref.ObjectId.empty()) return ref.ObjectId;
            return ToString(ref.Kind);
        }

        inline auto MakeNavigationLinkV2(const std::map<Str, FSemanticObjectOverviewV2> &objectsByKey,
                                         const FSemanticObjectRefV2 &target,
                                         Str detail = {},
                                         const bool bAmbient = false,
                                         const bool bReadonly = false,
                                         const bool bConflict = false,
                                         const bool bOverride = false) -> FSemanticNavigationLinkV2 {
            FSemanticNavigationLinkV2 link;
            link.Target = target;
            link.Detail = std::move(detail);
            link.bAmbient = bAmbient;
            link.bReadonly = bReadonly;
            link.bConflict = bConflict;
            link.bOverride = bOverride;

            if (const auto *object = FindObjectOverviewV2(objectsByKey, target); object != nullptr) {
                link.Label = object->DisplayLabel.empty() ? MakeObjectLabelFallbackV2(target) : object->DisplayLabel;
                if (link.bReadonly || object->bReadonly) link.bReadonly = true;
                if (link.bAmbient || object->bAmbient) link.bAmbient = true;
                if (link.bConflict || object->bConflict) link.bConflict = true;
                if (link.bOverride || object->bLocalOverride) link.bOverride = true;
            } else {
                link.Label = MakeObjectLabelFallbackV2(target);
            }

            return link;
        }

        inline auto AppendNavigationLinkUniqueV2(Vector<FSemanticNavigationLinkV2> &links,
                                                 FSemanticNavigationLinkV2 link) -> void {
            if (!link.Target.IsValid()) return;
            const auto it = std::find_if(links.begin(), links.end(), [&](const auto &existing) {
                return AreSemanticObjectRefsEqualV2(existing.Target, link.Target);
            });
            if (it == links.end()) {
                links.push_back(std::move(link));
                return;
            }

            if (it->Label.empty()) it->Label = link.Label;
            if (it->Detail.empty()) it->Detail = link.Detail;
            it->bAmbient = it->bAmbient || link.bAmbient;
            it->bReadonly = it->bReadonly || link.bReadonly;
            it->bConflict = it->bConflict || link.bConflict;
            it->bOverride = it->bOverride || link.bOverride;
        }

        inline auto AppendDiagnosticNavigationUniqueV2(Vector<FSemanticDiagnosticNavigationV2> &diagnostics,
                                                       FSemanticDiagnosticNavigationV2 diagnostic) -> void {
            const auto it = std::find_if(diagnostics.begin(), diagnostics.end(), [&](const auto &existing) {
                return existing.Diagnostic.Code == diagnostic.Diagnostic.Code &&
                    existing.Diagnostic.EntityId == diagnostic.Diagnostic.EntityId &&
                    existing.Diagnostic.Context == diagnostic.Diagnostic.Context &&
                    existing.Diagnostic.Message == diagnostic.Diagnostic.Message;
            });
            if (it != diagnostics.end()) return;
            diagnostics.push_back(std::move(diagnostic));
        }

        inline auto BuildDiagnosticNavigationV2(const FModelV2 &model,
                                                const FSemanticDiagnosticV2 &diagnostic,
                                                const FSemanticObjectRefV2 &defaultTarget)
            -> FSemanticDiagnosticNavigationV2 {
            FSemanticDiagnosticNavigationV2 navigation;
            navigation.Diagnostic = diagnostic;
            navigation.NavigateTo = ResolveObjectRefByIdV2(model, diagnostic.EntityId);
            if (!navigation.NavigateTo.has_value() && defaultTarget.IsValid()) {
                navigation.NavigateTo = defaultTarget;
            }
            return navigation;
        }

        inline auto JoinUsageLabelsV2(const StrVector &labels) -> Str {
            Str result;
            for (std::size_t i = 0; i < labels.size(); ++i) {
                if (i != 0) result += ", ";
                result += labels[i];
            }
            return result;
        }

        inline auto BuildDefinitionStatusLabelV2(const FDefinitionSemanticReportV2 *report,
                                                 const FDefinitionV2 &definition) -> Str {
            if (report != nullptr && report->bRoleMismatchesDeclared) return "mismatch / warning";
            if (definition.Metadata.contains("accepted_assumption_id")) return "materialized from assumption";
            return "local explicit";
        }

        inline auto BuildRelationStatusLabelV2(const FRelationSemanticReportV2 *report) -> Str {
            if (report == nullptr || report->InferredClass == ERelationSemanticClassV2::Unknown) return "local relation";
            return Str(ToString(report->InferredClass));
        }

        inline auto BuildVocabularyStatusLabelV2(const FResolvedBaseVocabularyEntryV2 &entry) -> Str {
            switch (entry.OverrideStatus) {
                case EVocabularyOverrideStatusV2::BuiltIn:
                    return "ambient readonly";
                case EVocabularyOverrideStatusV2::OverriddenLocally:
                    return "ambient readonly | overridden locally";
                case EVocabularyOverrideStatusV2::SpecializedLocally:
                    return "ambient readonly | specialized locally";
            }

            return "ambient readonly";
        }

        inline auto BuildAssumptionStatusLabelV2(const FSemanticAssumptionV2 &assumption) -> Str {
            Str label = !assumption.MaterializedDefinitionId.empty() ? "Materialized" : ToString(assumption.Status);
            if (assumption.bMismatchesDeclaredRole) label += " | mismatch / warning";
            else if (assumption.bMatchesDeclaredRole) label += " | agrees with local definition";
            return label;
        }

        inline auto CountUniqueDiagnosticsV2(const FModelSemanticReportV2 &report,
                                             std::size_t *pWarningCount = nullptr,
                                             std::size_t *pDeferredCount = nullptr,
                                             bool *pHasParseError = nullptr) -> void {
            std::set<Str> keys;
            std::size_t warningCount = 0;
            std::size_t deferredCount = 0;
            bool bHasParseError = false;

            const auto consumeDiagnostics = [&](const Vector<FSemanticDiagnosticV2> &diagnostics) {
                for (const auto &diagnostic : diagnostics) {
                    const auto key = diagnostic.Code + "|" + diagnostic.EntityId + "|" + diagnostic.Context + "|" + diagnostic.Message;
                    if (!keys.insert(key).second) continue;
                    if (diagnostic.Severity == EValidationSeverityV2::Warning) ++warningCount;
                    if (diagnostic.Code == "deferred_semantics") ++deferredCount;
                    if (diagnostic.Code == "parse_error") bHasParseError = true;
                }
            };

            consumeDiagnostics(report.Diagnostics);
            for (const auto &definition : report.Definitions) consumeDiagnostics(definition.Diagnostics);
            for (const auto &relation : report.Relations) consumeDiagnostics(relation.Diagnostics);

            if (pWarningCount != nullptr) *pWarningCount = warningCount;
            if (pDeferredCount != nullptr) *pDeferredCount = deferredCount;
            if (pHasParseError != nullptr) *pHasParseError = bHasParseError;
        }

        inline auto CountUnresolvedSymbolsV2(const FModelSemanticReportV2 &report) -> std::size_t {
            std::set<Str> keys;
            const auto consumeSymbols = [&](const Vector<FReferencedSymbolSemanticV2> &symbols) {
                for (const auto &symbol : symbols) {
                    if (symbol.bResolved || symbol.Origin != ESemanticOriginV2::Unresolved) continue;
                    keys.insert(symbol.SymbolText.empty() ? symbol.ReferenceId : symbol.SymbolText);
                }
            };

            for (const auto &definition : report.Definitions) consumeSymbols(definition.ReferencedSymbols);
            for (const auto &relation : report.Relations) consumeSymbols(relation.ReferencedSymbols);
            return keys.size();
        }

        inline auto DetermineSemanticHealthV2(const std::size_t validationErrors,
                                              const std::size_t unresolvedSymbols,
                                              const std::size_t warningCount,
                                              const std::size_t deferredCount,
                                              const std::size_t pendingAssumptions) -> ESemanticHealthV2 {
            if (validationErrors > 0 || unresolvedSymbols > 0) return ESemanticHealthV2::Error;
            const auto nonDeferredWarnings = warningCount > deferredCount ? warningCount - deferredCount : 0;
            if (nonDeferredWarnings > 0) return ESemanticHealthV2::Warning;
            if (deferredCount > 0 || pendingAssumptions > 0) return ESemanticHealthV2::Deferred;
            return ESemanticHealthV2::Ok;
        }

        inline auto AddSummaryLinkUniqueV2(Vector<FSemanticNavigationLinkV2> &links,
                                           const std::map<Str, FSemanticObjectOverviewV2> &objectsByKey,
                                           const FSemanticObjectRefV2 &ref) -> void {
            AppendNavigationLinkUniqueV2(links, MakeNavigationLinkV2(objectsByKey, ref));
        }

        inline auto BuildModelClassificationV2(const FModelV2 &model,
                                               const FModelSemanticReportV2 &report,
                                               const Vector<FResolvedBaseVocabularyEntryV2> &vocabularyEntries)
            -> FModelSemanticClassificationV2 {
            FModelSemanticClassificationV2 classification;

            const auto stateCount = static_cast<std::size_t>(std::count_if(model.Definitions.begin(), model.Definitions.end(), [](const auto &definition) {
                return definition.Kind == EDefinitionKindV2::StateVariable;
            }));
            const auto fieldCount = static_cast<std::size_t>(std::count_if(model.Definitions.begin(), model.Definitions.end(), [](const auto &definition) {
                return definition.Kind == EDefinitionKindV2::Field;
            }));
            const auto observableCount = static_cast<std::size_t>(std::count_if(model.Definitions.begin(), model.Definitions.end(), [](const auto &definition) {
                return definition.Kind == EDefinitionKindV2::ObservableSymbol;
            }));
            const auto constraintLikeCount = static_cast<std::size_t>(std::count_if(model.Relations.begin(), model.Relations.end(), [](const auto &relation) {
                return relation.Kind == ERelationKindV2::Constraint ||
                    relation.Kind == ERelationKindV2::Identity ||
                    relation.Kind == ERelationKindV2::SymbolicCondition;
            }));
            const auto pdeLikeCount = static_cast<std::size_t>(std::count_if(report.Relations.begin(), report.Relations.end(), [](const auto &relation) {
                return relation.InferredClass == ERelationSemanticClassV2::PDELike ||
                    relation.InferredClass == ERelationSemanticClassV2::FieldEquationLike;
            }));
            const auto odeLikeCount = static_cast<std::size_t>(std::count_if(report.Relations.begin(), report.Relations.end(), [](const auto &relation) {
                return relation.InferredClass == ERelationSemanticClassV2::DifferentialEquationLike ||
                    relation.InferredClass == ERelationSemanticClassV2::FirstOrderODELike ||
                    relation.InferredClass == ERelationSemanticClassV2::SecondOrderODELike;
            }));

            if (fieldCount > 0) {
                classification.ModelClass = fieldCount == 1 ? "scalar field model" : "field model";
            } else if (stateCount > 0) {
                classification.ModelClass = "finite-dimensional dynamical system";
            } else if (constraintLikeCount > 0 && constraintLikeCount >= std::max<std::size_t>(1, model.Relations.size() / 2)) {
                classification.ModelClass = "algebraic/constraint-heavy model";
            } else {
                classification.ModelClass = "symbolic model";
            }

            if (pdeLikeCount > 0) classification.Character = "PDE-like";
            else if (odeLikeCount > 0) classification.Character = "ODE-like";
            else if (constraintLikeCount > 0) classification.Character = "algebraic/constraint-heavy";
            else classification.Character = "mixed symbolic";

            if (observableCount > 0) classification.Traits.push_back("observable-rich");
            if (constraintLikeCount > 0) classification.Traits.push_back("constraint-heavy");
            if (fieldCount > 0) classification.Traits.push_back("field-oriented");
            if (std::any_of(vocabularyEntries.begin(), vocabularyEntries.end(), [](const auto &entry) {
                    return entry.OverrideStatus != EVocabularyOverrideStatusV2::BuiltIn;
                })) {
                classification.Traits.push_back("ambient-specialized");
            }

            return classification;
        }

    } // namespace Detail

    inline auto BuildModelSemanticOverviewV2(const FModelV2 &model) -> FModelSemanticOverviewV2 {
        FModelSemanticOverviewV2 overview;
        overview.VocabularyEntries = ResolveModelBaseVocabularyEntriesV2(model);
        overview.Validation = ValidateModelV2(model);
        overview.Report = BuildModelSemanticReportV2(model);

        std::map<Str, const FResolvedBaseVocabularyEntryV2 *> vocabularyByLocalDefinitionId;
        for (const auto &entry : overview.VocabularyEntries) {
            if (!entry.LocalDefinitionId.empty()) {
                vocabularyByLocalDefinitionId[entry.LocalDefinitionId] = &entry;
            }
        }

        for (const auto &entry : overview.VocabularyEntries) {
            FSemanticObjectOverviewV2 object;
            object.Ref = MakeVocabularyObjectRefV2(entry.Entry.EntryId);
            object.DisplayLabel = RenderBaseVocabularyEntryLabelV2(entry.Entry);
            object.CanonicalNotation = RenderBaseVocabularyEntryLatexV2(entry.Entry);
            object.KindLabel = ToString(entry.Entry.Kind);
            object.CategoryLabel = entry.Entry.DefinitionKind.has_value()
                ? ToString(*entry.Entry.DefinitionKind)
                : ToString(entry.Entry.Kind);
            object.OriginLabel = "BaseVocabulary";
            object.OriginDetail = entry.Entry.SourcePresetId;
            object.StatusLabel = Detail::BuildVocabularyStatusLabelV2(entry);
            object.Description = entry.Entry.Description.empty() ? entry.Entry.SemanticRoleSummary : entry.Entry.Description;
            object.bReadonly = true;
            object.bAmbient = true;
            object.DeclaredKind = entry.Entry.DefinitionKind;
            if (entry.OverrideStatus != EVocabularyOverrideStatusV2::BuiltIn) object.bLocalOverride = true;
            overview.ObjectsByKey[MakeSemanticObjectKeyV2(object.Ref)] = std::move(object);
        }

        for (const auto &definition : model.Definitions) {
            const auto *definitionReport = Detail::FindDefinitionSemanticReportV2(overview.Report, definition.DefinitionId);

            FSemanticObjectOverviewV2 object;
            object.Ref = MakeDefinitionObjectRefV2(definition.DefinitionId);
            object.DisplayLabel = RenderDefinitionLabelV2(definition);
            object.CanonicalNotation = MakeCanonicalDefinitionNotationV2(definition, &model);
            object.KindLabel = ToString(definition.Kind);
            object.CategoryLabel = "local symbol";
            object.OriginLabel = "LocalDefinition";
            object.OriginDetail = definition.Metadata.contains("accepted_assumption_id")
                ? definition.Metadata.at("accepted_assumption_id")
                : definition.DefinitionId;
            object.StatusLabel = Detail::BuildDefinitionStatusLabelV2(definitionReport, definition);
            object.Description = definition.Description;
            object.DeclaredKind = definition.Kind;
            if (definitionReport != nullptr) {
                object.InferredKind = definitionReport->InferredKind;
                object.bConflict = definitionReport->bRoleMismatchesDeclared;
                object.CategoryLabel = definitionReport->InferredKind.has_value()
                    ? ToString(*definitionReport->InferredKind)
                    : object.CategoryLabel;
                for (const auto &diagnostic : definitionReport->Diagnostics) {
                    Detail::AppendDiagnosticNavigationUniqueV2(
                        object.Diagnostics,
                        Detail::BuildDiagnosticNavigationV2(model, diagnostic, object.Ref));
                }
            }
            if (const auto it = vocabularyByLocalDefinitionId.find(definition.DefinitionId); it != vocabularyByLocalDefinitionId.end()) {
                object.bLocalOverride = true;
            }
            overview.ObjectsByKey[MakeSemanticObjectKeyV2(object.Ref)] = std::move(object);
        }

        for (const auto &relation : model.Relations) {
            const auto *relationReport = Detail::FindRelationSemanticReportV2(overview.Report, relation.RelationId);

            FSemanticObjectOverviewV2 object;
            object.Ref = MakeRelationObjectRefV2(relation.RelationId);
            object.DisplayLabel = relation.Name.empty() ? relation.RelationId : relation.Name;
            object.CanonicalNotation = MakeCanonicalRelationNotationV2(relation, &model);
            object.KindLabel = ToString(relation.Kind);
            object.CategoryLabel = "local relation";
            object.OriginLabel = "Relation-local usage";
            object.OriginDetail = relation.RelationId;
            object.StatusLabel = Detail::BuildRelationStatusLabelV2(relationReport);
            object.Description = relation.Description;
            if (relationReport != nullptr) {
                object.InferredRelationClass = relationReport->InferredClass;
                object.CategoryLabel = ToString(relationReport->InferredClass);
                for (const auto &diagnostic : relationReport->Diagnostics) {
                    Detail::AppendDiagnosticNavigationUniqueV2(
                        object.Diagnostics,
                        Detail::BuildDiagnosticNavigationV2(model, diagnostic, object.Ref));
                }
            }
            overview.ObjectsByKey[MakeSemanticObjectKeyV2(object.Ref)] = std::move(object);
        }

        for (const auto &assumption : overview.Report.Assumptions) {
            FSemanticObjectOverviewV2 object;
            object.Ref = MakeAssumptionObjectRefV2(assumption.AssumptionId);
            object.DisplayLabel = assumption.TargetSymbol.empty()
                ? (assumption.Category.empty() ? assumption.AssumptionId : assumption.Category)
                : assumption.TargetSymbol;
            object.CanonicalNotation = object.DisplayLabel;
            object.KindLabel = ToString(assumption.Kind);
            object.CategoryLabel = assumption.Category;
            object.OriginLabel = "Assumed / inferred semantics";
            object.OriginDetail = assumption.SourceId;
            object.StatusLabel = Detail::BuildAssumptionStatusLabelV2(assumption);
            object.Description = assumption.Detail;
            object.AssumptionStatus = assumption.Status;
            object.DeclaredKind = assumption.DeclaredKind;
            object.InferredKind = assumption.InferredKind;
            object.bConflict = assumption.bMismatchesDeclaredRole;
            overview.ObjectsByKey[MakeSemanticObjectKeyV2(object.Ref)] = std::move(object);
        }

        const auto appendLink = [&](const FSemanticObjectRefV2 &sourceRef,
                                    Vector<FSemanticNavigationLinkV2> FSemanticObjectOverviewV2::*member,
                                    FSemanticNavigationLinkV2 link) {
            if (auto *object = Detail::FindObjectOverviewV2(overview.ObjectsByKey, sourceRef); object != nullptr) {
                Detail::AppendNavigationLinkUniqueV2(object->*member, std::move(link));
            }
        };

        for (const auto &definition : model.Definitions) {
            if (!(definition.Metadata.contains("accepted_assumption_id"))) continue;
            const auto assumptionId = definition.Metadata.at("accepted_assumption_id");
            if (Detail::FindSemanticAssumptionV2(overview.Report, assumptionId) == nullptr) continue;

            const auto definitionRef = MakeDefinitionObjectRefV2(definition.DefinitionId);
            const auto assumptionRef = MakeAssumptionObjectRefV2(assumptionId);
            appendLink(
                definitionRef,
                &FSemanticObjectOverviewV2::SourceLinks,
                Detail::MakeNavigationLinkV2(overview.ObjectsByKey, assumptionRef, "materialized from assumption"));
        }

        for (const auto &definition : model.Definitions) {
            const auto sourceRef = MakeDefinitionObjectRefV2(definition.DefinitionId);
            const auto *definitionReport = Detail::FindDefinitionSemanticReportV2(overview.Report, definition.DefinitionId);

            auto addDefinitionDependency = [&](const Str &dependencyId, const Str &detail) {
                if (dependencyId.empty() || dependencyId == definition.DefinitionId) return;
                if (FindDefinitionByIdV2(model, dependencyId) == nullptr) return;
                const auto targetRef = MakeDefinitionObjectRefV2(dependencyId);
                appendLink(sourceRef, &FSemanticObjectOverviewV2::DependsOn, Detail::MakeNavigationLinkV2(overview.ObjectsByKey, targetRef, detail));
                appendLink(targetRef, &FSemanticObjectOverviewV2::UsedBy, Detail::MakeNavigationLinkV2(overview.ObjectsByKey, sourceRef, "uses this definition"));
            };

            for (const auto &dependencyId : definition.ArgumentDefinitionIds) {
                addDefinitionDependency(dependencyId, "argument dependency");
            }
            for (const auto &dependencyId : definition.DependencyDefinitionIds) {
                addDefinitionDependency(dependencyId, "declared dependency");
            }
            if (definitionReport != nullptr) {
                for (const auto &dependencyId : definitionReport->InferredArgumentDefinitionIds) {
                    addDefinitionDependency(dependencyId, "inferred dependency");
                }
                for (const auto &symbol : definitionReport->ReferencedSymbols) {
                    const auto targetRef = Detail::ResolveObjectRefForReferencedSymbolV2(symbol);
                    if (!targetRef.has_value()) continue;
                    const auto detail = symbol.SymbolText.empty() ? "referenced symbol" : symbol.SymbolText;
                    if (targetRef->Kind == ESemanticObjectKindV2::VocabularyEntry) {
                        appendLink(
                            sourceRef,
                            &FSemanticObjectOverviewV2::AmbientDependencies,
                            Detail::MakeNavigationLinkV2(overview.ObjectsByKey, *targetRef, detail, true, true));
                    } else if (!AreSemanticObjectRefsEqualV2(*targetRef, sourceRef)) {
                        appendLink(
                            sourceRef,
                            &FSemanticObjectOverviewV2::DependsOn,
                            Detail::MakeNavigationLinkV2(overview.ObjectsByKey, *targetRef, detail));
                    }
                    appendLink(
                        *targetRef,
                        &FSemanticObjectOverviewV2::UsedBy,
                        Detail::MakeNavigationLinkV2(overview.ObjectsByKey, sourceRef, "used by definition"));
                }
            }
            if (const auto it = vocabularyByLocalDefinitionId.find(definition.DefinitionId); it != vocabularyByLocalDefinitionId.end()) {
                const auto targetRef = MakeVocabularyObjectRefV2(it->second->Entry.EntryId);
                appendLink(
                    sourceRef,
                    &FSemanticObjectOverviewV2::LocalOverrides,
                    Detail::MakeNavigationLinkV2(
                        overview.ObjectsByKey,
                        targetRef,
                        ToString(it->second->OverrideStatus),
                        true,
                        true,
                        false,
                        true));
                appendLink(
                    targetRef,
                    &FSemanticObjectOverviewV2::LocalOverrides,
                    Detail::MakeNavigationLinkV2(
                        overview.ObjectsByKey,
                        sourceRef,
                        ToString(it->second->OverrideStatus),
                        true,
                        false,
                        false,
                        true));
            }
        }

        for (const auto &relation : model.Relations) {
            const auto sourceRef = MakeRelationObjectRefV2(relation.RelationId);
            const auto *relationReport = Detail::FindRelationSemanticReportV2(overview.Report, relation.RelationId);
            if (relationReport == nullptr) continue;

            for (const auto &symbol : relationReport->ReferencedSymbols) {
                const auto targetRef = Detail::ResolveObjectRefForReferencedSymbolV2(symbol);
                if (!targetRef.has_value()) continue;
                const auto detail = symbol.SymbolText.empty()
                    ? Detail::JoinUsageLabelsV2(symbol.UsageLabels)
                    : symbol.SymbolText;
                if (targetRef->Kind == ESemanticObjectKindV2::VocabularyEntry) {
                    appendLink(
                        sourceRef,
                        &FSemanticObjectOverviewV2::AmbientDependencies,
                        Detail::MakeNavigationLinkV2(overview.ObjectsByKey, *targetRef, detail, true, true));
                } else {
                    appendLink(
                        sourceRef,
                        &FSemanticObjectOverviewV2::DependsOn,
                        Detail::MakeNavigationLinkV2(overview.ObjectsByKey, *targetRef, detail));
                }
                appendLink(
                    *targetRef,
                    &FSemanticObjectOverviewV2::UsedBy,
                    Detail::MakeNavigationLinkV2(overview.ObjectsByKey, sourceRef, "used by relation"));
            }

            for (const auto &assumption : relationReport->Assumptions) {
                const auto assumptionRef = MakeAssumptionObjectRefV2(assumption.AssumptionId);
                appendLink(
                    sourceRef,
                    &FSemanticObjectOverviewV2::RelatedAssumptions,
                    Detail::MakeNavigationLinkV2(
                        overview.ObjectsByKey,
                        assumptionRef,
                        assumption.Detail,
                        false,
                        false,
                        assumption.bMismatchesDeclaredRole));
            }
        }

        for (const auto &assumption : overview.Report.Assumptions) {
            const auto sourceRef = MakeAssumptionObjectRefV2(assumption.AssumptionId);
            const auto sourceObjectRef = Detail::ResolveObjectRefByIdV2(model, assumption.SourceId);
            if (sourceObjectRef.has_value()) {
                appendLink(
                    sourceRef,
                    &FSemanticObjectOverviewV2::SourceLinks,
                    Detail::MakeNavigationLinkV2(overview.ObjectsByKey, *sourceObjectRef, "inferred from source"));
                appendLink(
                    *sourceObjectRef,
                    &FSemanticObjectOverviewV2::RelatedAssumptions,
                    Detail::MakeNavigationLinkV2(
                        overview.ObjectsByKey,
                        sourceRef,
                        assumption.Detail,
                        false,
                        false,
                        assumption.bMismatchesDeclaredRole));
            }

            const auto targetObjectRef = Detail::ResolveAssumptionTargetRefV2(model, assumption);
            if (targetObjectRef.has_value()) {
                appendLink(
                    sourceRef,
                    &FSemanticObjectOverviewV2::TargetLinks,
                    Detail::MakeNavigationLinkV2(
                        overview.ObjectsByKey,
                        *targetObjectRef,
                        assumption.bMismatchesDeclaredRole
                            ? "conflicts with explicit local meaning"
                            : "affected object",
                        targetObjectRef->Kind == ESemanticObjectKindV2::VocabularyEntry,
                        targetObjectRef->Kind == ESemanticObjectKindV2::VocabularyEntry,
                        assumption.bMismatchesDeclaredRole));
                appendLink(
                    *targetObjectRef,
                    &FSemanticObjectOverviewV2::RelatedAssumptions,
                    Detail::MakeNavigationLinkV2(
                        overview.ObjectsByKey,
                        sourceRef,
                        assumption.Detail,
                        targetObjectRef->Kind == ESemanticObjectKindV2::VocabularyEntry,
                        targetObjectRef->Kind == ESemanticObjectKindV2::VocabularyEntry,
                        assumption.bMismatchesDeclaredRole));
            }
        }

        for (const auto &relation : model.Relations) {
            const auto relationRef = MakeRelationObjectRefV2(relation.RelationId);
            const auto *relationObject = Detail::FindObjectOverviewV2(overview.ObjectsByKey, relationRef);
            if (relationObject == nullptr || relationObject->RelatedAssumptions.empty()) continue;

            for (const auto &dependency : relationObject->DependsOn) {
                for (const auto &assumption : relationObject->RelatedAssumptions) {
                    appendLink(
                        dependency.Target,
                        &FSemanticObjectOverviewV2::RelatedAssumptions,
                        Detail::MakeNavigationLinkV2(
                            overview.ObjectsByKey,
                            assumption.Target,
                            assumption.Detail,
                            false,
                            false,
                            assumption.bConflict));
                }
            }
            for (const auto &dependency : relationObject->AmbientDependencies) {
                for (const auto &assumption : relationObject->RelatedAssumptions) {
                    appendLink(
                        dependency.Target,
                        &FSemanticObjectOverviewV2::RelatedAssumptions,
                        Detail::MakeNavigationLinkV2(
                            overview.ObjectsByKey,
                            assumption.Target,
                            assumption.Detail,
                            true,
                            true,
                            assumption.bConflict));
                }
            }
        }

        overview.Status.ActiveBaseVocabularyId = model.BaseVocabulary.ActivePresetId;
        if (const auto *preset = FindBaseVocabularyPresetByIdV2(model.BaseVocabulary.ActivePresetId); preset != nullptr) {
            overview.Status.ActiveBaseVocabularyName = preset->Name;
        } else {
            overview.Status.ActiveBaseVocabularyName = model.BaseVocabulary.ActivePresetId;
        }

        overview.Status.ValidationErrorCount = overview.Validation.ErrorCount();
        Detail::CountUniqueDiagnosticsV2(
            overview.Report,
            &overview.Status.ValidationWarningCount,
            &overview.Status.DeferredCount,
            nullptr);
        overview.Status.UnresolvedSymbolCount = Detail::CountUnresolvedSymbolsV2(overview.Report);
        overview.Status.PendingAssumptionCount = static_cast<std::size_t>(std::count_if(
            overview.Report.Assumptions.begin(),
            overview.Report.Assumptions.end(),
            [](const auto &assumption) { return assumption.Status == EAssumptionStatusV2::Implicit; }));
        overview.Status.AcceptedAssumptionCount = static_cast<std::size_t>(std::count_if(
            overview.Report.Assumptions.begin(),
            overview.Report.Assumptions.end(),
            [](const auto &assumption) { return assumption.Status == EAssumptionStatusV2::Accepted; }));
        overview.Status.DismissedAssumptionCount = static_cast<std::size_t>(std::count_if(
            model.AssumptionStates.begin(),
            model.AssumptionStates.end(),
            [](const auto &state) { return state.Status == EAssumptionStatusV2::Dismissed; }));
        overview.Status.OverrideCount = static_cast<std::size_t>(std::count_if(
            overview.VocabularyEntries.begin(),
            overview.VocabularyEntries.end(),
            [](const auto &entry) { return entry.OverrideStatus == EVocabularyOverrideStatusV2::OverriddenLocally; }));
        overview.Status.SpecializationCount = static_cast<std::size_t>(std::count_if(
            overview.VocabularyEntries.begin(),
            overview.VocabularyEntries.end(),
            [](const auto &entry) { return entry.OverrideStatus == EVocabularyOverrideStatusV2::SpecializedLocally; }));
        overview.Status.ParseHealth = ESemanticHealthV2::Ok;
        overview.Status.SemanticHealth = Detail::DetermineSemanticHealthV2(
            overview.Status.ValidationErrorCount,
            overview.Status.UnresolvedSymbolCount,
            overview.Status.ValidationWarningCount,
            overview.Status.DeferredCount,
            overview.Status.PendingAssumptionCount);
        overview.Status.Classification = Detail::BuildModelClassificationV2(model, overview.Report, overview.VocabularyEntries);

        for (const auto &definition : model.Definitions) {
            const auto ref = MakeDefinitionObjectRefV2(definition.DefinitionId);
            switch (definition.Kind) {
                case EDefinitionKindV2::StateVariable:
                    Detail::AddSummaryLinkUniqueV2(overview.Status.CanonicalStateVariables, overview.ObjectsByKey, ref);
                    break;
                case EDefinitionKindV2::ScalarParameter:
                    Detail::AddSummaryLinkUniqueV2(overview.Status.Parameters, overview.ObjectsByKey, ref);
                    break;
                case EDefinitionKindV2::Field:
                    Detail::AddSummaryLinkUniqueV2(overview.Status.Fields, overview.ObjectsByKey, ref);
                    break;
                case EDefinitionKindV2::OperatorSymbol:
                    Detail::AddSummaryLinkUniqueV2(overview.Status.Operators, overview.ObjectsByKey, ref);
                    break;
                case EDefinitionKindV2::ObservableSymbol:
                    Detail::AddSummaryLinkUniqueV2(overview.Status.Observables, overview.ObjectsByKey, ref);
                    break;
                case EDefinitionKindV2::Coordinate:
                    break;
            }
        }

        for (const auto &relation : overview.Report.Relations) {
            for (const auto &symbol : relation.ReferencedSymbols) {
                if (symbol.Origin != ESemanticOriginV2::BaseVocabulary) continue;
                if (!(symbol.DeclaredKind.has_value() && *symbol.DeclaredKind == EDefinitionKindV2::OperatorSymbol) &&
                    !(symbol.InferredKind.has_value() && *symbol.InferredKind == EDefinitionKindV2::OperatorSymbol)) {
                    continue;
                }
                if (const auto ref = Detail::ResolveObjectRefForReferencedSymbolV2(symbol); ref.has_value()) {
                    Detail::AddSummaryLinkUniqueV2(overview.Status.Operators, overview.ObjectsByKey, *ref);
                }
            }
        }

        return overview;
    }

    namespace Detail {

        inline auto TextMentionsDraftObjectV2(const FSemanticDiagnosticV2 &diagnostic,
                                              const FSemanticObjectOverviewV2 &object) -> bool {
            if (!object.Ref.IsValid()) return false;
            if (!object.DisplayLabel.empty() && diagnostic.Message.find(object.DisplayLabel) != Str::npos) return true;
            if (!object.CanonicalNotation.empty() && diagnostic.Message.find(object.CanonicalNotation) != Str::npos) return true;
            if (!object.Ref.ObjectId.empty() && diagnostic.Message.find(object.Ref.ObjectId) != Str::npos) return true;
            return false;
        }

        inline auto AppendDraftNavigationLinkIfObjectExistsV2(
            FModelSemanticOverviewV2 &overview,
            const FSemanticObjectRefV2 &sourceRef,
            Vector<FSemanticNavigationLinkV2> FSemanticObjectOverviewV2::*member,
            const FSemanticObjectRefV2 &targetRef,
            const Str &detail,
            const bool bAmbient = false,
            const bool bReadonly = false,
            const bool bConflict = false,
            const bool bOverride = false) -> void {
            auto *sourceObject = FindObjectOverviewV2(overview.ObjectsByKey, sourceRef);
            if (sourceObject == nullptr) return;
            if (FindObjectOverviewV2(overview.ObjectsByKey, targetRef) == nullptr) return;
            AppendNavigationLinkUniqueV2(
                sourceObject->*member,
                MakeNavigationLinkV2(
                    overview.ObjectsByKey,
                    targetRef,
                    detail,
                    bAmbient,
                    bReadonly,
                    bConflict,
                    bOverride));
        }

        inline auto BuildDraftMaterializationObjectOverviewV2(const FDefinitionV2 &definition,
                                                              const FSemanticAssumptionV2 &assumption,
                                                              const FModelV2 &model)
            -> FSemanticObjectOverviewV2 {
            FSemanticObjectOverviewV2 object;
            object.Ref = MakeDefinitionObjectRefV2(definition.DefinitionId);
            object.DisplayLabel = RenderDefinitionLabelV2(definition);
            object.CanonicalNotation = RenderDialectDefinitionV2(definition, &model);
            object.KindLabel = ToString(definition.Kind);
            object.CategoryLabel = "draft materialization";
            object.OriginLabel = "Assumed / inferred semantics";
            object.OriginDetail = assumption.AssumptionId;
            object.StatusLabel = "draft only";
            object.Description = assumption.Detail;
            object.DeclaredKind = definition.Kind;
            object.InferredKind = definition.Kind;
            return object;
        }

        inline auto RebindDiagnosticNavigationsToDraftObjectsV2(FModelSemanticOverviewV2 &overview) -> void {
            auto collectCandidateRefs = [&](const FSemanticObjectOverviewV2 &object) {
                Vector<FSemanticObjectRefV2> refs;
                const auto collect = [&](const Vector<FSemanticNavigationLinkV2> &links) {
                    for (const auto &link : links) {
                        if (!link.Target.IsValid()) continue;
                        if (std::find_if(refs.begin(), refs.end(), [&](const auto &existing) {
                                return AreSemanticObjectRefsEqualV2(existing, link.Target);
                            }) != refs.end()) {
                            continue;
                        }
                        refs.push_back(link.Target);
                    }
                };

                collect(object.RelatedAssumptions);
                collect(object.TargetLinks);
                collect(object.SourceLinks);
                collect(object.DependsOn);
                collect(object.AmbientDependencies);
                collect(object.UsedBy);
                collect(object.LocalOverrides);
                return refs;
            };

            for (auto &[key, object] : overview.ObjectsByKey) {
                (void) key;
                const auto candidateRefs = collectCandidateRefs(object);
                for (auto &diagnosticNavigation : object.Diagnostics) {
                    if (diagnosticNavigation.NavigateTo.has_value() &&
                        !AreSemanticObjectRefsEqualV2(*diagnosticNavigation.NavigateTo, object.Ref)) {
                        continue;
                    }

                    for (const auto &candidateRef : candidateRefs) {
                        const auto *candidateObject = FindObjectOverviewV2(overview.ObjectsByKey, candidateRef);
                        if (candidateObject == nullptr) continue;
                        if (!TextMentionsDraftObjectV2(diagnosticNavigation.Diagnostic, *candidateObject)) continue;
                        diagnosticNavigation.NavigateTo = candidateRef;
                        break;
                    }
                }
            }
        }

    } // namespace Detail

    inline auto BuildDraftSemanticOverviewV2(const FModelV2 &model) -> FModelSemanticOverviewV2 {
        auto overview = BuildModelSemanticOverviewV2(model);

        for (const auto &assumption : overview.Report.Assumptions) {
            const auto materializationPreview = BuildAssumptionMaterializationPreviewV2(model, assumption, &overview);
            if (!materializationPreview.ProposedDefinition.has_value()) continue;

            const auto &draftDefinition = *materializationPreview.ProposedDefinition;
            auto object = Detail::BuildDraftMaterializationObjectOverviewV2(draftDefinition, assumption, model);
            const auto draftDefinitionRef = object.Ref;
            overview.ObjectsByKey[MakeSemanticObjectKeyV2(draftDefinitionRef)] = std::move(object);

            Detail::AppendDraftNavigationLinkIfObjectExistsV2(
                overview,
                MakeAssumptionObjectRefV2(assumption.AssumptionId),
                &FSemanticObjectOverviewV2::TargetLinks,
                draftDefinitionRef,
                "would materialize into local definition");
            Detail::AppendDraftNavigationLinkIfObjectExistsV2(
                overview,
                draftDefinitionRef,
                &FSemanticObjectOverviewV2::SourceLinks,
                MakeAssumptionObjectRefV2(assumption.AssumptionId),
                "materialized from draft assumption");

            for (const auto &dependencyId : draftDefinition.ArgumentDefinitionIds) {
                if (FindDefinitionByIdV2(model, dependencyId) == nullptr) continue;
                const auto dependencyRef = MakeDefinitionObjectRefV2(dependencyId);
                Detail::AppendDraftNavigationLinkIfObjectExistsV2(
                    overview,
                    draftDefinitionRef,
                    &FSemanticObjectOverviewV2::DependsOn,
                    dependencyRef,
                    "proposed dependency");
                Detail::AppendDraftNavigationLinkIfObjectExistsV2(
                    overview,
                    dependencyRef,
                    &FSemanticObjectOverviewV2::UsedBy,
                    draftDefinitionRef,
                    "used by draft materialization");
            }

            if (const auto sourceObjectRef = Detail::ResolveObjectRefByIdV2(model, assumption.SourceId); sourceObjectRef.has_value()) {
                Detail::AppendDraftNavigationLinkIfObjectExistsV2(
                    overview,
                    draftDefinitionRef,
                    &FSemanticObjectOverviewV2::UsedBy,
                    *sourceObjectRef,
                    "materializes a symbol referenced by this draft");
                Detail::AppendDraftNavigationLinkIfObjectExistsV2(
                    overview,
                    *sourceObjectRef,
                    &FSemanticObjectOverviewV2::DependsOn,
                    draftDefinitionRef,
                    "draft-created symbol");
            }
        }

        Detail::RebindDiagnosticNavigationsToDraftObjectsV2(overview);
        return overview;
    }

    inline auto ResolveReferencedSymbolNavigationTargetV2(const FModelSemanticOverviewV2 &overview,
                                                          const FReferencedSymbolSemanticV2 &symbol)
        -> TOptional<FSemanticObjectRefV2> {
        return Detail::ResolveDraftObjectRefForReferencedSymbolV2(overview, symbol);
    }

    namespace Detail {

        inline auto CollectObjectReferencedSymbolsV2(const FModelSemanticOverviewV2 &overview,
                                                     const FSemanticObjectRefV2 &ref)
            -> Vector<FReferencedSymbolSemanticV2> {
            if (ref.Kind == ESemanticObjectKindV2::Definition) {
                if (const auto *report = FindDefinitionSemanticReportV2(overview.Report, ref.ObjectId); report != nullptr) {
                    return report->ReferencedSymbols;
                }
            } else if (ref.Kind == ESemanticObjectKindV2::Relation) {
                if (const auto *report = FindRelationSemanticReportV2(overview.Report, ref.ObjectId); report != nullptr) {
                    return report->ReferencedSymbols;
                }
            }

            return {};
        }

        inline auto CollectObjectDiagnosticsV2(const FModelSemanticOverviewV2 &overview,
                                               const FSemanticObjectRefV2 &ref)
            -> Vector<FSemanticDiagnosticV2> {
            if (ref.Kind == ESemanticObjectKindV2::Definition) {
                if (const auto *report = FindDefinitionSemanticReportV2(overview.Report, ref.ObjectId); report != nullptr) {
                    return report->Diagnostics;
                }
            } else if (ref.Kind == ESemanticObjectKindV2::Relation) {
                if (const auto *report = FindRelationSemanticReportV2(overview.Report, ref.ObjectId); report != nullptr) {
                    return report->Diagnostics;
                }
            }

            return {};
        }

        inline auto CollectObjectInterpretationV2(const FModelSemanticOverviewV2 &overview,
                                                  const FSemanticObjectRefV2 &ref) -> Str {
            if (ref.Kind == ESemanticObjectKindV2::Definition) {
                if (const auto *report = FindDefinitionSemanticReportV2(overview.Report, ref.ObjectId); report != nullptr) {
                    return report->NormalizedInterpretation;
                }
            } else if (ref.Kind == ESemanticObjectKindV2::Relation) {
                if (const auto *report = FindRelationSemanticReportV2(overview.Report, ref.ObjectId); report != nullptr) {
                    return report->NormalizedInterpretation;
                }
            }

            return {};
        }

        inline auto CollectObjectInferredKindV2(const FModelSemanticOverviewV2 &overview,
                                                const FSemanticObjectRefV2 &ref)
            -> TOptional<EDefinitionKindV2> {
            if (ref.Kind != ESemanticObjectKindV2::Definition) return std::nullopt;
            if (const auto *report = FindDefinitionSemanticReportV2(overview.Report, ref.ObjectId); report != nullptr) {
                return report->InferredKind;
            }
            return std::nullopt;
        }

        inline auto CollectObjectInferredClassV2(const FModelSemanticOverviewV2 &overview,
                                                 const FSemanticObjectRefV2 &ref)
            -> TOptional<ERelationSemanticClassV2> {
            if (ref.Kind != ESemanticObjectKindV2::Relation) return std::nullopt;
            if (const auto *report = FindRelationSemanticReportV2(overview.Report, ref.ObjectId); report != nullptr) {
                return TOptional<ERelationSemanticClassV2>{report->InferredClass};
            }
            return std::nullopt;
        }

        inline auto DiffNavigationLinksV2(const Vector<FSemanticNavigationLinkV2> &canonicalLinks,
                                          const Vector<FSemanticNavigationLinkV2> &draftLinks)
            -> Vector<FSemanticNavigationDeltaV2> {
            Vector<FSemanticNavigationDeltaV2> deltas;

            std::map<Str, const FSemanticNavigationLinkV2 *> canonicalByKey;
            for (const auto &link : canonicalLinks) {
                canonicalByKey[BuildNavigationLinkKeyV2(link)] = &link;
            }

            for (const auto &link : draftLinks) {
                const auto key = BuildNavigationLinkKeyV2(link);
                if (const auto it = canonicalByKey.find(key); it != canonicalByKey.end()) {
                    const auto *canonical = it->second;
                    const bool bChanged =
                        canonical->Detail != link.Detail ||
                        canonical->bAmbient != link.bAmbient ||
                        canonical->bReadonly != link.bReadonly ||
                        canonical->bConflict != link.bConflict ||
                        canonical->bOverride != link.bOverride;
                    if (bChanged) {
                        deltas.push_back(FSemanticNavigationDeltaV2{
                            .Kind = ESemanticDeltaKindV2::Changed,
                            .Link = link,
                            .PreviousDetail = canonical->Detail,
                            .NewDetail = link.Detail
                        });
                    }
                    canonicalByKey.erase(it);
                    continue;
                }

                deltas.push_back(FSemanticNavigationDeltaV2{
                    .Kind = ESemanticDeltaKindV2::Added,
                    .Link = link,
                    .NewDetail = link.Detail
                });
            }

            for (const auto &entry : canonicalByKey) {
                const auto *canonical = entry.second;
                deltas.push_back(FSemanticNavigationDeltaV2{
                    .Kind = ESemanticDeltaKindV2::Removed,
                    .Link = *canonical,
                    .PreviousDetail = canonical->Detail
                });
            }

            return deltas;
        }

        inline auto DiffReferencedSymbolsV2(const Vector<FReferencedSymbolSemanticV2> &canonicalSymbols,
                                            const Vector<FReferencedSymbolSemanticV2> &draftSymbols)
            -> Vector<FReferencedSymbolDeltaV2> {
            Vector<FReferencedSymbolDeltaV2> deltas;

            std::map<Str, const FReferencedSymbolSemanticV2 *> canonicalByKey;
            for (const auto &symbol : canonicalSymbols) {
                canonicalByKey[BuildReferencedSymbolKeyV2(symbol)] = &symbol;
            }

            for (const auto &symbol : draftSymbols) {
                const auto key = BuildReferencedSymbolKeyV2(symbol);
                if (const auto it = canonicalByKey.find(key); it != canonicalByKey.end()) {
                    const auto *canonical = it->second;
                    const bool bResolutionChanged = canonical->bResolved != symbol.bResolved;
                    const bool bOriginChanged = canonical->Origin != symbol.Origin;
                    const bool bDeclaredKindChanged = !OptionalValuesEqualV2(canonical->DeclaredKind, symbol.DeclaredKind);
                    const bool bInferredKindChanged = !OptionalValuesEqualV2(canonical->InferredKind, symbol.InferredKind);
                    const bool bArgumentDependenciesChanged =
                        canonical->InferredArgumentDefinitionIds != symbol.InferredArgumentDefinitionIds;
                    if (bResolutionChanged || bOriginChanged || bDeclaredKindChanged || bInferredKindChanged || bArgumentDependenciesChanged) {
                        FReferencedSymbolDeltaV2 delta;
                        delta.Kind = ESemanticDeltaKindV2::Changed;
                        delta.Symbol = symbol;
                        delta.bResolutionChanged = bResolutionChanged;
                        delta.bOriginChanged = bOriginChanged;
                        delta.bDeclaredKindChanged = bDeclaredKindChanged;
                        delta.bInferredKindChanged = bInferredKindChanged;
                        delta.bArgumentDependenciesChanged = bArgumentDependenciesChanged;
                        delta.bCanonicalResolved = canonical->bResolved;
                        delta.CanonicalOrigin = canonical->Origin;
                        delta.CanonicalDeclaredKind = canonical->DeclaredKind;
                        delta.CanonicalInferredKind = canonical->InferredKind;
                        delta.CanonicalArgumentDefinitionIds = canonical->InferredArgumentDefinitionIds;
                        deltas.push_back(std::move(delta));
                    }
                    canonicalByKey.erase(it);
                    continue;
                }

                deltas.push_back(FReferencedSymbolDeltaV2{
                    .Kind = ESemanticDeltaKindV2::Added,
                    .Symbol = symbol
                });
            }

            for (const auto &entry : canonicalByKey) {
                const auto *canonical = entry.second;
                FReferencedSymbolDeltaV2 delta;
                delta.Kind = ESemanticDeltaKindV2::Removed;
                delta.Symbol = *canonical;
                delta.bCanonicalResolved = canonical->bResolved;
                delta.CanonicalOrigin = canonical->Origin;
                delta.CanonicalDeclaredKind = canonical->DeclaredKind;
                delta.CanonicalInferredKind = canonical->InferredKind;
                delta.CanonicalArgumentDefinitionIds = canonical->InferredArgumentDefinitionIds;
                deltas.push_back(std::move(delta));
            }

            return deltas;
        }

        inline auto DiffDiagnosticsV2(const Vector<FSemanticDiagnosticV2> &canonicalDiagnostics,
                                      const Vector<FSemanticDiagnosticV2> &draftDiagnostics)
            -> Vector<FSemanticDiagnosticDeltaV2> {
            Vector<FSemanticDiagnosticDeltaV2> deltas;

            std::map<Str, const FSemanticDiagnosticV2 *> canonicalByKey;
            for (const auto &diagnostic : canonicalDiagnostics) {
                canonicalByKey[BuildDiagnosticKeyV2(diagnostic)] = &diagnostic;
            }

            for (const auto &diagnostic : draftDiagnostics) {
                const auto key = BuildDiagnosticKeyV2(diagnostic);
                if (const auto it = canonicalByKey.find(key); it != canonicalByKey.end()) {
                    canonicalByKey.erase(it);
                    continue;
                }

                deltas.push_back(FSemanticDiagnosticDeltaV2{
                    .Kind = ESemanticDeltaKindV2::Added,
                    .Diagnostic = diagnostic
                });
            }

            for (const auto &entry : canonicalByKey) {
                deltas.push_back(FSemanticDiagnosticDeltaV2{
                    .Kind = ESemanticDeltaKindV2::Removed,
                    .Diagnostic = *entry.second
                });
            }

            return deltas;
        }

        inline auto CollectRelatedAssumptionIdsV2(const FSemanticObjectOverviewV2 *object) -> std::set<Str> {
            std::set<Str> assumptionIds;
            if (object == nullptr) return assumptionIds;

            for (const auto &link : object->RelatedAssumptions) {
                if (link.Target.Kind != ESemanticObjectKindV2::Assumption || link.Target.ObjectId.empty()) continue;
                assumptionIds.insert(link.Target.ObjectId);
            }

            if (object->Ref.Kind == ESemanticObjectKindV2::Assumption && object->Ref.IsValid()) {
                assumptionIds.insert(object->Ref.ObjectId);
            }

            return assumptionIds;
        }

        inline auto DiffAssumptionsV2(const FModelSemanticOverviewV2 &canonicalOverview,
                                      const FModelSemanticOverviewV2 &draftOverview,
                                      const std::set<Str> &assumptionIds)
            -> Vector<FSemanticAssumptionDeltaV2> {
            Vector<FSemanticAssumptionDeltaV2> deltas;

            for (const auto &assumptionId : assumptionIds) {
                const auto *canonical = FindSemanticAssumptionV2(canonicalOverview.Report, assumptionId);
                const auto *draft = FindSemanticAssumptionV2(draftOverview.Report, assumptionId);

                if (canonical == nullptr && draft == nullptr) continue;
                if (canonical == nullptr && draft != nullptr) {
                    deltas.push_back(FSemanticAssumptionDeltaV2{
                        .Kind = ESemanticDeltaKindV2::Added,
                        .Assumption = *draft
                    });
                    continue;
                }
                if (canonical != nullptr && draft == nullptr) {
                    deltas.push_back(FSemanticAssumptionDeltaV2{
                        .Kind = ESemanticDeltaKindV2::Removed,
                        .Assumption = *canonical,
                        .CanonicalStatus = canonical->Status,
                        .CanonicalMaterializedDefinitionId = canonical->MaterializedDefinitionId
                    });
                    continue;
                }

                const bool bStatusChanged = canonical->Status != draft->Status;
                const bool bMaterializationChanged = canonical->MaterializedDefinitionId != draft->MaterializedDefinitionId;
                const bool bMeaningChanged =
                    canonical->Category != draft->Category ||
                    canonical->Detail != draft->Detail ||
                    canonical->bWouldCreateDefinition != draft->bWouldCreateDefinition ||
                    canonical->bMatchesDeclaredRole != draft->bMatchesDeclaredRole ||
                    canonical->bMismatchesDeclaredRole != draft->bMismatchesDeclaredRole ||
                    canonical->ProposedArgumentDefinitionIds != draft->ProposedArgumentDefinitionIds ||
                    !OptionalValuesEqualV2(canonical->DeclaredKind, draft->DeclaredKind) ||
                    !OptionalValuesEqualV2(canonical->InferredKind, draft->InferredKind);

                if (bStatusChanged || bMaterializationChanged || bMeaningChanged) {
                    deltas.push_back(FSemanticAssumptionDeltaV2{
                        .Kind = ESemanticDeltaKindV2::Changed,
                        .Assumption = *draft,
                        .CanonicalStatus = canonical->Status,
                        .CanonicalMaterializedDefinitionId = canonical->MaterializedDefinitionId,
                        .bStatusChanged = bStatusChanged,
                        .bMaterializationChanged = bMaterializationChanged
                    });
                }
            }

            return deltas;
        }

        inline auto DetermineAssumptionLifecyclePhaseV2(const FModelV2 &model,
                                                        const FSemanticAssumptionV2 &assumption)
            -> EAssumptionLifecyclePhaseV2 {
            if (!assumption.MaterializedDefinitionId.empty() &&
                FindDefinitionByIdV2(model, assumption.MaterializedDefinitionId) != nullptr) {
                return EAssumptionLifecyclePhaseV2::Materialized;
            }

            switch (assumption.Status) {
                case EAssumptionStatusV2::Accepted: return EAssumptionLifecyclePhaseV2::Accepted;
                case EAssumptionStatusV2::Dismissed: return EAssumptionLifecyclePhaseV2::Dismissed;
                case EAssumptionStatusV2::Implicit: return EAssumptionLifecyclePhaseV2::Implicit;
            }

            return EAssumptionLifecyclePhaseV2::Implicit;
        }

    } // namespace Detail

    inline auto BuildModelSemanticDraftDeltaV2(const FModelSemanticOverviewV2 &canonicalOverview,
                                               const FModelSemanticOverviewV2 &draftOverview,
                                               const FSemanticObjectRefV2 &targetRef) -> FModelSemanticDraftDeltaV2 {
        FModelSemanticDraftDeltaV2 delta;
        delta.bAvailable = true;
        delta.CanonicalStatus = canonicalOverview.Status;
        delta.DraftStatus = draftOverview.Status;
        delta.CanonicalInterpretation = Detail::CollectObjectInterpretationV2(canonicalOverview, targetRef);
        delta.DraftInterpretation = Detail::CollectObjectInterpretationV2(draftOverview, targetRef);
        delta.CanonicalInferredKind = Detail::CollectObjectInferredKindV2(canonicalOverview, targetRef);
        delta.DraftInferredKind = Detail::CollectObjectInferredKindV2(draftOverview, targetRef);
        delta.CanonicalRelationClass = Detail::CollectObjectInferredClassV2(canonicalOverview, targetRef);
        delta.DraftRelationClass = Detail::CollectObjectInferredClassV2(draftOverview, targetRef);

        const auto *canonicalObject = canonicalOverview.FindObject(targetRef);
        const auto *draftObject = draftOverview.FindObject(targetRef);
        if (canonicalObject != nullptr) delta.bCanonicalConflict = canonicalObject->bConflict;
        if (draftObject != nullptr) delta.bDraftConflict = draftObject->bConflict;

        delta.ReferencedSymbolChanges = Detail::DiffReferencedSymbolsV2(
            Detail::CollectObjectReferencedSymbolsV2(canonicalOverview, targetRef),
            Detail::CollectObjectReferencedSymbolsV2(draftOverview, targetRef));
        delta.DiagnosticChanges = Detail::DiffDiagnosticsV2(
            Detail::CollectObjectDiagnosticsV2(canonicalOverview, targetRef),
            Detail::CollectObjectDiagnosticsV2(draftOverview, targetRef));

        delta.DependencyChanges = Detail::DiffNavigationLinksV2(
            canonicalObject != nullptr ? canonicalObject->DependsOn : Vector<FSemanticNavigationLinkV2>{},
            draftObject != nullptr ? draftObject->DependsOn : Vector<FSemanticNavigationLinkV2>{});
        delta.AmbientDependencyChanges = Detail::DiffNavigationLinksV2(
            canonicalObject != nullptr ? canonicalObject->AmbientDependencies : Vector<FSemanticNavigationLinkV2>{},
            draftObject != nullptr ? draftObject->AmbientDependencies : Vector<FSemanticNavigationLinkV2>{});
        delta.UsedByChanges = Detail::DiffNavigationLinksV2(
            canonicalObject != nullptr ? canonicalObject->UsedBy : Vector<FSemanticNavigationLinkV2>{},
            draftObject != nullptr ? draftObject->UsedBy : Vector<FSemanticNavigationLinkV2>{});
        delta.AssumptionLinkChanges = Detail::DiffNavigationLinksV2(
            canonicalObject != nullptr ? canonicalObject->RelatedAssumptions : Vector<FSemanticNavigationLinkV2>{},
            draftObject != nullptr ? draftObject->RelatedAssumptions : Vector<FSemanticNavigationLinkV2>{});
        delta.OverrideChanges = Detail::DiffNavigationLinksV2(
            canonicalObject != nullptr ? canonicalObject->LocalOverrides : Vector<FSemanticNavigationLinkV2>{},
            draftObject != nullptr ? draftObject->LocalOverrides : Vector<FSemanticNavigationLinkV2>{});

        auto assumptionIds = Detail::CollectRelatedAssumptionIdsV2(canonicalObject);
        const auto draftAssumptionIds = Detail::CollectRelatedAssumptionIdsV2(draftObject);
        assumptionIds.insert(draftAssumptionIds.begin(), draftAssumptionIds.end());
        delta.AssumptionChanges = Detail::DiffAssumptionsV2(canonicalOverview, draftOverview, assumptionIds);

        return delta;
    }

    inline auto BuildAssumptionMaterializationPreviewV2(const FModelV2 &model,
                                                        const FSemanticAssumptionV2 &assumption,
                                                        const FModelSemanticOverviewV2 *pOverview)
        -> FAssumptionMaterializationPreviewV2 {
        FAssumptionMaterializationPreviewV2 preview;
        preview.bAvailable = true;
        preview.bWouldCreateDefinition = assumption.bWouldCreateDefinition && assumption.InferredKind.has_value();
        preview.ProposedType = assumption.ProposedType;

        FModelSemanticOverviewV2 ownedOverview;
        const auto *overview = pOverview;
        if (overview == nullptr) {
            ownedOverview = BuildModelSemanticOverviewV2(model);
            overview = &ownedOverview;
        }

        preview.LifecyclePhase = Detail::DetermineAssumptionLifecyclePhaseV2(model, assumption);
        preview.LifecycleLabel = ToString(preview.LifecyclePhase);

        if (!assumption.MaterializedDefinitionId.empty() &&
            FindDefinitionByIdV2(model, assumption.MaterializedDefinitionId) != nullptr) {
            preview.bAlreadyMaterialized = true;
            preview.MaterializedObjectRef = MakeDefinitionObjectRefV2(assumption.MaterializedDefinitionId);
            preview.OutcomeLabel = "Already materialized into explicit local model content.";
        } else if (preview.bWouldCreateDefinition) {
            if (const auto proposedDefinition = Detail::BuildDefinitionFromAssumptionV2(model, assumption); proposedDefinition.has_value()) {
                preview.ProposedDefinition = *proposedDefinition;
                preview.SuggestedDefinitionId = proposedDefinition->DefinitionId;
                preview.ProposedNotation = RenderDialectDefinitionV2(*proposedDefinition, &model);
                preview.ProposedType = proposedDefinition->DeclaredType;
                preview.OutcomeLabel = "Accepting will create a new local definition.";
                for (const auto &dependencyId : proposedDefinition->ArgumentDefinitionIds) {
                    if (overview->FindObject(MakeDefinitionObjectRefV2(dependencyId)) == nullptr) continue;
                    Detail::AppendNavigationLinkUniqueV2(
                        preview.ProposedDependencies,
                        Detail::MakeNavigationLinkV2(
                            overview->ObjectsByKey,
                            MakeDefinitionObjectRefV2(dependencyId),
                            "proposed dependency"));
                }
            }
        } else if (assumption.Status == EAssumptionStatusV2::Accepted) {
            preview.OutcomeLabel = "Accepted into canonical semantics without creating a new local definition.";
        } else if (assumption.Status == EAssumptionStatusV2::Dismissed) {
            preview.OutcomeLabel = "Dismissed; no canonical artifact will be created.";
        } else if (assumption.bMismatchesDeclaredRole) {
            preview.OutcomeLabel = "Implicit proposal that conflicts with explicit local meaning.";
        } else if (assumption.bMatchesDeclaredRole) {
            preview.OutcomeLabel = "Implicit proposal already aligned with explicit local meaning.";
        } else {
            preview.OutcomeLabel = "Implicit proposal; accepting canonizes its semantic consequence.";
        }

        return preview;
    }

    inline auto BuildSemanticSelectionContextV2(const FModelSemanticOverviewV2 &overview,
                                                const FSemanticObjectRefV2 &selected) -> FSemanticSelectionContextV2 {
        FSemanticSelectionContextV2 context;
        context.Selected = selected;

        const auto *object = overview.FindObject(selected);
        if (object == nullptr) return context;

        const auto consumeLinks = [](const Vector<FSemanticNavigationLinkV2> &links, std::set<Str> &keys) {
            for (const auto &link : links) {
                if (!link.Target.IsValid()) continue;
                keys.insert(MakeSemanticObjectKeyV2(link.Target));
            }
        };

        consumeLinks(object->DependsOn, context.DependencyKeys);
        consumeLinks(object->AmbientDependencies, context.AmbientDependencyKeys);
        consumeLinks(object->UsedBy, context.UsedByKeys);
        consumeLinks(object->RelatedAssumptions, context.RelatedAssumptionKeys);
        consumeLinks(object->SourceLinks, context.SourceKeys);
        consumeLinks(object->TargetLinks, context.TargetKeys);
        consumeLinks(object->LocalOverrides, context.OverrideKeys);

        if (object->bConflict) context.ConflictKeys.insert(MakeSemanticObjectKeyV2(selected));
        if (selected.Kind == ESemanticObjectKindV2::Assumption && object->bConflict) {
            consumeLinks(object->TargetLinks, context.ConflictKeys);
        }
        for (const auto &assumptionLink : object->RelatedAssumptions) {
            if (const auto *assumptionObject = overview.FindObject(assumptionLink.Target);
                assumptionObject != nullptr && assumptionObject->bConflict) {
                context.ConflictKeys.insert(MakeSemanticObjectKeyV2(assumptionLink.Target));
            }
        }

        return context;
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
        Detail::CollectTypeEvidenceV2(*definition.DeclaredType, model, evidenceByKey);
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

        const auto canonicalOverview = BuildModelSemanticOverviewV2(model);
        preview.SemanticOverview = BuildDraftSemanticOverviewV2(previewModel);
        preview.SemanticDelta = BuildModelSemanticDraftDeltaV2(
            canonicalOverview,
            *preview.SemanticOverview,
            MakeDefinitionObjectRefV2(selectedDefinition.DefinitionId));

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

    inline auto PreviewNewDefinitionV2(const FModelV2 &model,
                                       const Str &definitionId,
                                       const EDefinitionKindV2 kind,
                                       const ECoordinateRoleV2 coordinateRole,
                                       const EOperatorApplicationStyleV2 operatorStyle,
                                       const Str &draftNotation,
                                       const Str &displayName = {},
                                       const Str &description = {}) -> FDefinitionDraftPreviewV2 {
        FDefinitionDraftPreviewV2 preview;
        preview.DefinitionId = definitionId;
        preview.CanonicalNotation = {};
        preview.DraftNotation = draftNotation;

        if (definitionId.empty()) {
            preview.Diagnostics.push_back(FSemanticDiagnosticV2{
                .Severity = EValidationSeverityV2::Error,
                .Code = "missing_definition_id",
                .EntityId = definitionId,
                .Context = "Definition",
                .Message = "definition id is empty"
            });
            preview.bHasBlockingErrors = true;
            return preview;
        }

        if (FindDefinitionByIdV2(model, definitionId) != nullptr) {
            preview.Diagnostics.push_back(FSemanticDiagnosticV2{
                .Severity = EValidationSeverityV2::Error,
                .Code = "duplicate_definition_id",
                .EntityId = definitionId,
                .Context = "Definition",
                .Message = "definition id '" + definitionId + "' already exists"
            });
            preview.bHasBlockingErrors = true;
            return preview;
        }

        const auto context = FNotationContextV2::FromModel(model);
        const auto parsed = ParseDefinitionNotationV2(draftNotation, &context);
        if (!parsed.IsOk()) {
            preview.ParseError = parsed.Error;
            preview.Diagnostics.push_back(FSemanticDiagnosticV2{
                .Severity = EValidationSeverityV2::Error,
                .Code = "parse_error",
                .EntityId = definitionId,
                .Context = "Definition",
                .Message = parsed.Error.Message
            });
            preview.bHasBlockingErrors = true;
            return preview;
        }

        preview.bParseOk = true;

        FDefinitionV2 proposedDefinition;
        proposedDefinition.DefinitionId = definitionId;
        proposedDefinition.Kind = kind;
        proposedDefinition.CoordinateRole =
            kind == EDefinitionKindV2::Coordinate ? coordinateRole : ECoordinateRoleV2::Generic;
        proposedDefinition.OperatorStyle = operatorStyle;
        proposedDefinition.Symbol = parsed.Value->CanonicalSymbol;
        proposedDefinition.PreferredNotation = parsed.Value->SourceSymbolText;
        proposedDefinition.SourceText = draftNotation;
        proposedDefinition.DisplayName = !displayName.empty()
            ? displayName
            : (!proposedDefinition.PreferredNotation.empty() ? proposedDefinition.PreferredNotation : proposedDefinition.DefinitionId);
        proposedDefinition.Description = description;
        if (parsed.Value->bMembership && parsed.Value->MembershipSpace.has_value()) {
            proposedDefinition.DeclaredType = MakeSpaceTypeV2(*parsed.Value->MembershipSpace);
        } else if (parsed.Value->DeclaredType.has_value()) {
            proposedDefinition.DeclaredType = *parsed.Value->DeclaredType;
        }

        preview.ProposedDefinition = proposedDefinition;
        preview.NormalizedProjection = RenderDialectDefinitionV2(proposedDefinition, &model);

        auto previewModel = model;
        previewModel.Definitions.push_back(proposedDefinition);

        const auto canonicalOverview = BuildModelSemanticOverviewV2(model);
        preview.SemanticOverview = BuildDraftSemanticOverviewV2(previewModel);
        preview.SemanticDelta = BuildModelSemanticDraftDeltaV2(
            canonicalOverview,
            *preview.SemanticOverview,
            MakeDefinitionObjectRefV2(definitionId));

        const auto semanticReport = BuildModelSemanticReportV2(previewModel);
        if (const auto it = std::find_if(
                semanticReport.Definitions.rbegin(),
                semanticReport.Definitions.rend(),
                [&](const auto &report) { return report.DefinitionId == definitionId; });
            it != semanticReport.Definitions.rend()) {
            preview.NormalizedInterpretation = it->NormalizedInterpretation;
            preview.InferredKind = it->InferredKind;
            preview.Assumptions = it->Assumptions;
            preview.Diagnostics = it->Diagnostics;
        }

        preview.ReferencedSymbols = CollectDefinitionPreviewReferencesV2(previewModel, proposedDefinition);
        for (const auto &symbol : preview.ReferencedSymbols) {
            Detail::AppendInferenceDiagnosticsForSymbolV2(
                symbol,
                definitionId,
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

        const auto canonicalOverview = BuildModelSemanticOverviewV2(model);
        preview.SemanticOverview = BuildDraftSemanticOverviewV2(previewModel);
        preview.SemanticDelta = BuildModelSemanticDraftDeltaV2(
            canonicalOverview,
            *preview.SemanticOverview,
            MakeRelationObjectRefV2(selectedRelation.RelationId));

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

    inline auto PreviewNewRelationV2(const FModelV2 &model,
                                     const Str &relationId,
                                     const ERelationKindV2 kind,
                                     const Str &draftNotation,
                                     const Str &name = {},
                                     const Str &description = {}) -> FRelationDraftPreviewV2 {
        FRelationDraftPreviewV2 preview;
        preview.RelationId = relationId;
        preview.CanonicalNotation = {};
        preview.DraftNotation = draftNotation;

        if (relationId.empty()) {
            preview.Diagnostics.push_back(FSemanticDiagnosticV2{
                .Severity = EValidationSeverityV2::Error,
                .Code = "missing_relation_id",
                .EntityId = relationId,
                .Context = "Relation",
                .Message = "relation id is empty"
            });
            preview.bHasBlockingErrors = true;
            return preview;
        }

        if (FindRelationByIdV2(model, relationId) != nullptr) {
            preview.Diagnostics.push_back(FSemanticDiagnosticV2{
                .Severity = EValidationSeverityV2::Error,
                .Code = "duplicate_relation_id",
                .EntityId = relationId,
                .Context = "Relation",
                .Message = "relation id '" + relationId + "' already exists"
            });
            preview.bHasBlockingErrors = true;
            return preview;
        }

        const auto context = FNotationContextV2::FromModel(model);
        const auto parsed = ParseRelationNotationV2(
            relationId,
            kind,
            draftNotation,
            &context);
        if (!parsed.IsOk()) {
            preview.ParseError = parsed.Error;
            preview.Diagnostics.push_back(FSemanticDiagnosticV2{
                .Severity = EValidationSeverityV2::Error,
                .Code = "parse_error",
                .EntityId = relationId,
                .Context = "Relation",
                .Message = parsed.Error.Message
            });
            preview.bHasBlockingErrors = true;
            return preview;
        }

        preview.bParseOk = true;

        auto proposedRelation = *parsed.Value;
        proposedRelation.Name = name;
        proposedRelation.Description = description;
        preview.ProposedRelation = proposedRelation;

        auto previewModel = model;
        previewModel.Relations.push_back(proposedRelation);

        const auto canonicalOverview = BuildModelSemanticOverviewV2(model);
        preview.SemanticOverview = BuildDraftSemanticOverviewV2(previewModel);
        preview.SemanticDelta = BuildModelSemanticDraftDeltaV2(
            canonicalOverview,
            *preview.SemanticOverview,
            MakeRelationObjectRefV2(relationId));

        const auto semanticReport = BuildModelSemanticReportV2(previewModel);
        if (const auto it = std::find_if(
                semanticReport.Relations.rbegin(),
                semanticReport.Relations.rend(),
                [&](const auto &report) { return report.RelationId == relationId; });
            it != semanticReport.Relations.rend()) {
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

    inline auto ApplyNewDefinitionPreviewV2(FModelV2 &model,
                                            const FDefinitionDraftPreviewV2 &preview,
                                            FModelChangeRecordV2 *pOutChangeRecord = nullptr) -> bool {
        if (!preview.bCanApply || !preview.ProposedDefinition.has_value()) return false;
        if (preview.DefinitionId.empty() || FindDefinitionByIdV2(model, preview.DefinitionId) != nullptr) return false;

        model.Definitions.push_back(*preview.ProposedDefinition);

        FModelChangeRecordV2 changeRecord;
        changeRecord.ObjectKind = EModelObjectKindV2::Definition;
        changeRecord.ObjectId = preview.DefinitionId;
        changeRecord.PreviousCanonicalNotation = {};
        changeRecord.NewCanonicalNotation = preview.DraftNotation;
        changeRecord.Origin = EModelChangeOriginV2::DirectEdit;
        ApplyModelChangeRecordV2(model, changeRecord);
        if (pOutChangeRecord != nullptr) *pOutChangeRecord = model.ChangeLog.back();
        return true;
    }

    inline auto ApplyNewRelationPreviewV2(FModelV2 &model,
                                          const FRelationDraftPreviewV2 &preview,
                                          FModelChangeRecordV2 *pOutChangeRecord = nullptr) -> bool {
        if (!preview.bCanApply || !preview.ProposedRelation.has_value()) return false;
        if (preview.RelationId.empty() || FindRelationByIdV2(model, preview.RelationId) != nullptr) return false;

        model.Relations.push_back(*preview.ProposedRelation);

        FModelChangeRecordV2 changeRecord;
        changeRecord.ObjectKind = EModelObjectKindV2::Relation;
        changeRecord.ObjectId = preview.RelationId;
        changeRecord.PreviousCanonicalNotation = {};
        changeRecord.NewCanonicalNotation = preview.DraftNotation;
        changeRecord.Origin = EModelChangeOriginV2::DirectEdit;
        ApplyModelChangeRecordV2(model, changeRecord);
        if (pOutChangeRecord != nullptr) *pOutChangeRecord = model.ChangeLog.back();
        return true;
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

        if (const auto proposedDefinition = Detail::BuildDefinitionFromAssumptionV2(model, assumption);
            proposedDefinition.has_value()) {
            model.Definitions.push_back(*proposedDefinition);

            SetAssumptionStatusV2(
                model,
                assumption.AssumptionId,
                EAssumptionStatusV2::Accepted,
                proposedDefinition->DefinitionId);

            FModelChangeRecordV2 changeRecord;
            changeRecord.ObjectKind = EModelObjectKindV2::Definition;
            changeRecord.ObjectId = proposedDefinition->DefinitionId;
            changeRecord.PreviousCanonicalNotation = {};
            changeRecord.NewCanonicalNotation = RenderDialectDefinitionV2(*proposedDefinition, &model);
            changeRecord.Origin = EModelChangeOriginV2::AcceptedInference;
            changeRecord.SourceId = assumption.AssumptionId;
            ApplyModelChangeRecordV2(model, changeRecord);
            if (pOutChangeRecord != nullptr) *pOutChangeRecord = model.ChangeLog.back();
            return true;
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
