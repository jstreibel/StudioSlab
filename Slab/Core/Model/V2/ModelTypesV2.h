#ifndef STUDIOSLAB_MODEL_TYPES_V2_H
#define STUDIOSLAB_MODEL_TYPES_V2_H

#include "Utils/Optional.h"
#include "Utils/Types.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <sstream>
#include <utility>

namespace Slab::Core::Model::V2 {

    enum class EDefinitionKindV2 : unsigned char {
        ScalarParameter,
        Coordinate,
        StateVariable,
        Field,
        OperatorSymbol,
        ObservableSymbol
    };

    enum class ECoordinateRoleV2 : unsigned char {
        Generic,
        Time,
        Space,
        Spacetime
    };

    enum class EOperatorApplicationStyleV2 : unsigned char {
        Prefix,
        FunctionLike
    };

    enum class ERelationKindV2 : unsigned char {
        Equation,
        DifferentialEquation,
        OperatorEquation,
        Constraint,
        Identity,
        SymbolicCondition
    };

    enum class EExpressionKindV2 : unsigned char {
        Symbol,
        Literal,
        Unary,
        Binary,
        FunctionApplication,
        OperatorApplication,
        Derivative
    };

    enum class EUnaryOperatorV2 : unsigned char {
        Plus,
        Minus
    };

    enum class EBinaryOperatorV2 : unsigned char {
        Add,
        Subtract,
        Multiply,
        Divide,
        Power
    };

    enum class EDerivativeFlavorV2 : unsigned char {
        Ordinary,
        Partial
    };

    enum class ETypeExprKindV2 : unsigned char {
        Space,
        Function
    };

    enum class ESpaceKindV2 : unsigned char {
        RealNumbers,
        ComplexNumbers,
        NamedSpace
    };

    enum class EValidationSeverityV2 : unsigned char {
        Error,
        Warning
    };

    enum class EAssumptionStatusV2 : unsigned char {
        Implicit,
        Accepted,
        Dismissed
    };

    enum class EModelObjectKindV2 : unsigned char {
        Definition,
        Relation,
        Assumption
    };

    enum class EModelChangeOriginV2 : unsigned char {
        DirectEdit,
        AcceptedInference
    };

    enum class EBaseVocabularyEntryKindV2 : unsigned char {
        ScalarSet,
        Operator,
        RelationSymbol,
        NotationConvention,
        FunctionConvention,
        ArithmeticConvention
    };

    enum class EVocabularyOverrideStatusV2 : unsigned char {
        BuiltIn,
        OverriddenLocally,
        SpecializedLocally
    };

    enum class ESemanticOriginV2 : unsigned char {
        LocalDefinition,
        BaseVocabulary,
        Assumed,
        Unresolved
    };

    struct FExpressionV2;
    using FExpressionPtrV2 = TPointer<FExpressionV2>;

    struct FReferenceV2 {
        Str ReferenceId;
        Str SymbolText;

        [[nodiscard]] auto IsBound() const -> bool {
            return !ReferenceId.empty();
        }

        [[nodiscard]] auto DisplayText() const -> Str {
            if (!SymbolText.empty()) return SymbolText;
            return ReferenceId;
        }
    };

    struct FSpaceExprV2 {
        ESpaceKindV2 Kind = ESpaceKindV2::RealNumbers;
        Str Name;
        TOptional<FExpressionPtrV2> Dimension;
        Str SourceText;
    };

    struct FTypeExprV2 {
        ETypeExprKindV2 Kind = ETypeExprKindV2::Space;
        FSpaceExprV2 Space;
        Vector<FTypeExprV2> Domain;
        TPointer<FTypeExprV2> Codomain = nullptr;
        Str SourceText;
    };

    struct FBaseVocabularyEntryV2 {
        Str EntryId;
        Str Symbol;
        Str PreferredNotation;
        Str PreviewLatex;
        Str DisplayName;
        Str Description;
        EBaseVocabularyEntryKindV2 Kind = EBaseVocabularyEntryKindV2::NotationConvention;
        Str SemanticRoleSummary;
        Str SourcePresetId;
        TOptional<EDefinitionKindV2> DefinitionKind;
        EOperatorApplicationStyleV2 OperatorStyle = EOperatorApplicationStyleV2::Prefix;
        TOptional<FTypeExprV2> DeclaredType;
        bool bGenericUnaryScalarFunctionOperator = false;
        Str SourceText;
        StrVector Tags;
        std::map<Str, Str> Metadata;
    };

    struct FDefinitionV2 {
        Str DefinitionId;
        Str Symbol;
        Str PreferredNotation;
        Str DisplayName;
        Str Description;
        EDefinitionKindV2 Kind = EDefinitionKindV2::ScalarParameter;
        ECoordinateRoleV2 CoordinateRole = ECoordinateRoleV2::Generic;
        EOperatorApplicationStyleV2 OperatorStyle = EOperatorApplicationStyleV2::Prefix;
        TOptional<FTypeExprV2> DeclaredType;
        Vector<Str> ArgumentDefinitionIds;
        Vector<Str> DependencyDefinitionIds;
        Str SourceText;
        StrVector Tags;
        std::map<Str, Str> Metadata;
    };

    struct FExpressionV2 {
        EExpressionKindV2 Kind = EExpressionKindV2::Literal;
        Str SourceText;
        FReferenceV2 Reference;
        Str LiteralText;
        EUnaryOperatorV2 UnaryOperator = EUnaryOperatorV2::Plus;
        EBinaryOperatorV2 BinaryOperator = EBinaryOperatorV2::Add;
        EDerivativeFlavorV2 DerivativeFlavor = EDerivativeFlavorV2::Ordinary;
        Vector<FReferenceV2> DerivativeVariables;
        Vector<FExpressionPtrV2> Children;
        bool bImplicitApplication = false;
    };

    struct FRelationV2 {
        Str RelationId;
        Str Name;
        Str Description;
        ERelationKindV2 Kind = ERelationKindV2::Equation;
        FExpressionPtrV2 Left = nullptr;
        FExpressionPtrV2 Right = nullptr;
        Str SourceText;
        StrVector Tags;
        std::map<Str, Str> Metadata;
    };

    struct FAssumptionStateV2 {
        Str AssumptionId;
        EAssumptionStatusV2 Status = EAssumptionStatusV2::Implicit;
        Str SourceSignature;
        Str MaterializedDefinitionId;
        std::map<Str, Str> Metadata;
    };

    struct FModelChangeRecordV2 {
        Str ChangeId;
        EModelObjectKindV2 ObjectKind = EModelObjectKindV2::Definition;
        Str ObjectId;
        Str PreviousCanonicalNotation;
        Str NewCanonicalNotation;
        EModelChangeOriginV2 Origin = EModelChangeOriginV2::DirectEdit;
        Str SourceId;
        std::map<Str, Str> Metadata;
    };

    struct FBaseVocabularyPresetV2 {
        Str PresetId;
        Str Name;
        Str Description;
        Str ParentPresetId;
        Vector<FBaseVocabularyEntryV2> Entries;
        StrVector Tags;
        std::map<Str, Str> Metadata;
    };

    struct FModelBaseVocabularyV2 {
        Str ActivePresetId;
        std::map<Str, Str> Metadata;
    };

    struct FResolvedBaseVocabularyEntryV2 {
        FBaseVocabularyEntryV2 Entry;
        EVocabularyOverrideStatusV2 OverrideStatus = EVocabularyOverrideStatusV2::BuiltIn;
        Str LocalDefinitionId;
    };

    struct FModelInitialConditionAssignmentV2 {
        Str StateDefinitionId;
        FExpressionPtrV2 ValueExpression = nullptr;
        Str Description;
        std::map<Str, Str> Metadata;
    };

    struct FModelInitialConditionSetV2 {
        FExpressionPtrV2 TimeExpression = nullptr;
        Vector<FModelInitialConditionAssignmentV2> Assignments;
        Str Description;
        std::map<Str, Str> Metadata;
    };

    struct FModelV2 {
        Str ModelId;
        Str Name;
        Str Description;
        FModelBaseVocabularyV2 BaseVocabulary;
        Vector<FDefinitionV2> Definitions;
        Vector<FRelationV2> Relations;
        TOptional<FModelInitialConditionSetV2> InitialConditions;
        Vector<FAssumptionStateV2> AssumptionStates;
        Vector<FModelChangeRecordV2> ChangeLog;
        StrVector Tags;
        std::map<Str, Str> Metadata;
    };

    struct FValidationMessageV2 {
        EValidationSeverityV2 Severity = EValidationSeverityV2::Error;
        Str EntityId;
        Str Context;
        Str Message;
    };

    struct FValidationResultV2 {
        Vector<FValidationMessageV2> Messages;

        [[nodiscard]] auto ErrorCount() const -> std::size_t {
            return static_cast<std::size_t>(std::count_if(
                Messages.begin(),
                Messages.end(),
                [](const auto &message) { return message.Severity == EValidationSeverityV2::Error; }));
        }

        [[nodiscard]] auto IsOk() const -> bool {
            return ErrorCount() == 0;
        }

        auto Add(const EValidationSeverityV2 severity,
                 Str entityId,
                 Str context,
                 Str message) -> void {
            Messages.push_back(FValidationMessageV2{
                severity,
                std::move(entityId),
                std::move(context),
                std::move(message)
            });
        }
    };

    inline auto ToString(const EDefinitionKindV2 value) -> const char * {
        switch (value) {
            case EDefinitionKindV2::ScalarParameter: return "ScalarParameter";
            case EDefinitionKindV2::Coordinate: return "Coordinate";
            case EDefinitionKindV2::StateVariable: return "StateVariable";
            case EDefinitionKindV2::Field: return "Field";
            case EDefinitionKindV2::OperatorSymbol: return "OperatorSymbol";
            case EDefinitionKindV2::ObservableSymbol: return "ObservableSymbol";
        }

        return "Unknown";
    }

    inline auto ToString(const ECoordinateRoleV2 value) -> const char * {
        switch (value) {
            case ECoordinateRoleV2::Generic: return "Generic";
            case ECoordinateRoleV2::Time: return "Time";
            case ECoordinateRoleV2::Space: return "Space";
            case ECoordinateRoleV2::Spacetime: return "Spacetime";
        }

        return "Unknown";
    }

    inline auto ToString(const EOperatorApplicationStyleV2 value) -> const char * {
        switch (value) {
            case EOperatorApplicationStyleV2::Prefix: return "Prefix";
            case EOperatorApplicationStyleV2::FunctionLike: return "FunctionLike";
        }

        return "Unknown";
    }

    inline auto ToString(const ERelationKindV2 value) -> const char * {
        switch (value) {
            case ERelationKindV2::Equation: return "Equation";
            case ERelationKindV2::DifferentialEquation: return "DifferentialEquation";
            case ERelationKindV2::OperatorEquation: return "OperatorEquation";
            case ERelationKindV2::Constraint: return "Constraint";
            case ERelationKindV2::Identity: return "Identity";
            case ERelationKindV2::SymbolicCondition: return "SymbolicCondition";
        }

        return "Unknown";
    }

    inline auto ToString(const EExpressionKindV2 value) -> const char * {
        switch (value) {
            case EExpressionKindV2::Symbol: return "Symbol";
            case EExpressionKindV2::Literal: return "Literal";
            case EExpressionKindV2::Unary: return "Unary";
            case EExpressionKindV2::Binary: return "Binary";
            case EExpressionKindV2::FunctionApplication: return "FunctionApplication";
            case EExpressionKindV2::OperatorApplication: return "OperatorApplication";
            case EExpressionKindV2::Derivative: return "Derivative";
        }

        return "Unknown";
    }

    inline auto ToString(const EUnaryOperatorV2 value) -> const char * {
        switch (value) {
            case EUnaryOperatorV2::Plus: return "+";
            case EUnaryOperatorV2::Minus: return "-";
        }

        return "?";
    }

    inline auto ToString(const EBinaryOperatorV2 value) -> const char * {
        switch (value) {
            case EBinaryOperatorV2::Add: return "+";
            case EBinaryOperatorV2::Subtract: return "-";
            case EBinaryOperatorV2::Multiply: return "*";
            case EBinaryOperatorV2::Divide: return "/";
            case EBinaryOperatorV2::Power: return "^";
        }

        return "?";
    }

    inline auto ToString(const EDerivativeFlavorV2 value) -> const char * {
        switch (value) {
            case EDerivativeFlavorV2::Ordinary: return "Ordinary";
            case EDerivativeFlavorV2::Partial: return "Partial";
        }

        return "Unknown";
    }

    inline auto ToString(const ETypeExprKindV2 value) -> const char * {
        switch (value) {
            case ETypeExprKindV2::Space: return "Space";
            case ETypeExprKindV2::Function: return "Function";
        }

        return "Unknown";
    }

    inline auto ToString(const ESpaceKindV2 value) -> const char * {
        switch (value) {
            case ESpaceKindV2::RealNumbers: return "RealNumbers";
            case ESpaceKindV2::ComplexNumbers: return "ComplexNumbers";
            case ESpaceKindV2::NamedSpace: return "NamedSpace";
        }

        return "Unknown";
    }

    inline auto ToString(const EValidationSeverityV2 value) -> const char * {
        switch (value) {
            case EValidationSeverityV2::Error: return "Error";
            case EValidationSeverityV2::Warning: return "Warning";
        }

        return "Unknown";
    }

    inline auto ToString(const EAssumptionStatusV2 value) -> const char * {
        switch (value) {
            case EAssumptionStatusV2::Implicit: return "Implicit";
            case EAssumptionStatusV2::Accepted: return "Accepted";
            case EAssumptionStatusV2::Dismissed: return "Dismissed";
        }

        return "Unknown";
    }

    inline auto ToString(const EModelObjectKindV2 value) -> const char * {
        switch (value) {
            case EModelObjectKindV2::Definition: return "Definition";
            case EModelObjectKindV2::Relation: return "Relation";
            case EModelObjectKindV2::Assumption: return "Assumption";
        }

        return "Unknown";
    }

    inline auto ToString(const EModelChangeOriginV2 value) -> const char * {
        switch (value) {
            case EModelChangeOriginV2::DirectEdit: return "DirectEdit";
            case EModelChangeOriginV2::AcceptedInference: return "AcceptedInference";
        }

        return "Unknown";
    }

    inline auto ToString(const EBaseVocabularyEntryKindV2 value) -> const char * {
        switch (value) {
            case EBaseVocabularyEntryKindV2::ScalarSet: return "ScalarSet";
            case EBaseVocabularyEntryKindV2::Operator: return "Operator";
            case EBaseVocabularyEntryKindV2::RelationSymbol: return "RelationSymbol";
            case EBaseVocabularyEntryKindV2::NotationConvention: return "NotationConvention";
            case EBaseVocabularyEntryKindV2::FunctionConvention: return "FunctionConvention";
            case EBaseVocabularyEntryKindV2::ArithmeticConvention: return "ArithmeticConvention";
        }

        return "Unknown";
    }

    inline auto ToString(const EVocabularyOverrideStatusV2 value) -> const char * {
        switch (value) {
            case EVocabularyOverrideStatusV2::BuiltIn: return "BuiltIn";
            case EVocabularyOverrideStatusV2::OverriddenLocally: return "OverriddenLocally";
            case EVocabularyOverrideStatusV2::SpecializedLocally: return "SpecializedLocally";
        }

        return "Unknown";
    }

    inline auto ToString(const ESemanticOriginV2 value) -> const char * {
        switch (value) {
            case ESemanticOriginV2::LocalDefinition: return "LocalDefinition";
            case ESemanticOriginV2::BaseVocabulary: return "BaseVocabulary";
            case ESemanticOriginV2::Assumed: return "Assumed";
            case ESemanticOriginV2::Unresolved: return "Unresolved";
        }

        return "Unknown";
    }

    inline auto TrimAsciiCopyV2(Str value) -> Str {
        const auto isSpace = [](const unsigned char c) {
            return std::isspace(c) != 0;
        };

        const auto begin = std::find_if_not(value.begin(), value.end(), isSpace);
        if (begin == value.end()) return {};

        const auto end = std::find_if_not(value.rbegin(), value.rend(), isSpace).base();
        return Str(begin, end);
    }

    inline auto NormalizeSymbolAliasTextV2(Str alias) -> Str {
        alias = TrimAsciiCopyV2(std::move(alias));
        while (!alias.empty() && alias.front() == '\\') {
            alias.erase(alias.begin());
        }
        return alias;
    }

    inline auto MakeReferenceV2(Str referenceId, Str symbolText = {}) -> FReferenceV2 {
        return FReferenceV2{std::move(referenceId), std::move(symbolText)};
    }

    inline auto MakeRealSpaceV2(TOptional<FExpressionPtrV2> dimension = std::nullopt) -> FSpaceExprV2 {
        FSpaceExprV2 space;
        space.Kind = ESpaceKindV2::RealNumbers;
        space.Name = "R";
        space.Dimension = std::move(dimension);
        return space;
    }

    inline auto MakeComplexSpaceV2(TOptional<FExpressionPtrV2> dimension = std::nullopt) -> FSpaceExprV2 {
        FSpaceExprV2 space;
        space.Kind = ESpaceKindV2::ComplexNumbers;
        space.Name = "C";
        space.Dimension = std::move(dimension);
        return space;
    }

    inline auto MakeNamedSpaceV2(Str name, TOptional<FExpressionPtrV2> dimension = std::nullopt) -> FSpaceExprV2 {
        FSpaceExprV2 space;
        space.Kind = ESpaceKindV2::NamedSpace;
        space.Name = std::move(name);
        space.Dimension = std::move(dimension);
        return space;
    }

    inline auto MakeSpaceTypeV2(FSpaceExprV2 space) -> FTypeExprV2 {
        FTypeExprV2 type;
        type.Kind = ETypeExprKindV2::Space;
        type.Space = std::move(space);
        return type;
    }

    inline auto MakeFunctionTypeV2(Vector<FTypeExprV2> domain, FTypeExprV2 codomain) -> FTypeExprV2 {
        FTypeExprV2 type;
        type.Kind = ETypeExprKindV2::Function;
        type.Domain = std::move(domain);
        type.Codomain = New<FTypeExprV2>(std::move(codomain));
        return type;
    }

    inline auto MakeSymbolExprV2(FReferenceV2 reference, Str sourceText = {}) -> FExpressionPtrV2 {
        auto expression = New<FExpressionV2>();
        expression->Kind = EExpressionKindV2::Symbol;
        expression->Reference = std::move(reference);
        expression->SourceText = std::move(sourceText);
        return expression;
    }

    inline auto MakeLiteralExprV2(Str literalText, Str sourceText = {}) -> FExpressionPtrV2 {
        auto expression = New<FExpressionV2>();
        expression->Kind = EExpressionKindV2::Literal;
        expression->LiteralText = std::move(literalText);
        expression->SourceText = std::move(sourceText);
        return expression;
    }

    inline auto MakeUnaryExprV2(const EUnaryOperatorV2 op,
                                FExpressionPtrV2 operand,
                                Str sourceText = {}) -> FExpressionPtrV2 {
        auto expression = New<FExpressionV2>();
        expression->Kind = EExpressionKindV2::Unary;
        expression->UnaryOperator = op;
        expression->Children = {std::move(operand)};
        expression->SourceText = std::move(sourceText);
        return expression;
    }

    inline auto MakeBinaryExprV2(const EBinaryOperatorV2 op,
                                 FExpressionPtrV2 left,
                                 FExpressionPtrV2 right,
                                 const bool implicitApplication = false,
                                 Str sourceText = {}) -> FExpressionPtrV2 {
        auto expression = New<FExpressionV2>();
        expression->Kind = EExpressionKindV2::Binary;
        expression->BinaryOperator = op;
        expression->Children = {std::move(left), std::move(right)};
        expression->bImplicitApplication = implicitApplication;
        expression->SourceText = std::move(sourceText);
        return expression;
    }

    inline auto MakeFunctionApplicationV2(FReferenceV2 callee,
                                          Vector<FExpressionPtrV2> arguments,
                                          Str sourceText = {}) -> FExpressionPtrV2 {
        auto expression = New<FExpressionV2>();
        expression->Kind = EExpressionKindV2::FunctionApplication;
        expression->Reference = std::move(callee);
        expression->Children = std::move(arguments);
        expression->SourceText = std::move(sourceText);
        return expression;
    }

    inline auto MakeOperatorApplicationV2(FReferenceV2 operatorRef,
                                          Vector<FExpressionPtrV2> arguments,
                                          Str sourceText = {}) -> FExpressionPtrV2 {
        auto expression = New<FExpressionV2>();
        expression->Kind = EExpressionKindV2::OperatorApplication;
        expression->Reference = std::move(operatorRef);
        expression->Children = std::move(arguments);
        expression->SourceText = std::move(sourceText);
        return expression;
    }

    inline auto MakeDerivativeExprV2(const EDerivativeFlavorV2 flavor,
                                     Vector<FReferenceV2> variables,
                                     FExpressionPtrV2 target,
                                     Str sourceText = {}) -> FExpressionPtrV2 {
        auto expression = New<FExpressionV2>();
        expression->Kind = EExpressionKindV2::Derivative;
        expression->DerivativeFlavor = flavor;
        expression->DerivativeVariables = std::move(variables);
        expression->Children = {std::move(target)};
        expression->SourceText = std::move(sourceText);
        return expression;
    }

    inline auto AddExprV2(FExpressionPtrV2 left, FExpressionPtrV2 right, const bool implicit = false) -> FExpressionPtrV2 {
        return MakeBinaryExprV2(EBinaryOperatorV2::Add, std::move(left), std::move(right), implicit);
    }

    inline auto SubExprV2(FExpressionPtrV2 left, FExpressionPtrV2 right) -> FExpressionPtrV2 {
        return MakeBinaryExprV2(EBinaryOperatorV2::Subtract, std::move(left), std::move(right));
    }

    inline auto MulExprV2(FExpressionPtrV2 left, FExpressionPtrV2 right, const bool implicit = true) -> FExpressionPtrV2 {
        return MakeBinaryExprV2(EBinaryOperatorV2::Multiply, std::move(left), std::move(right), implicit);
    }

    inline auto DivExprV2(FExpressionPtrV2 left, FExpressionPtrV2 right) -> FExpressionPtrV2 {
        return MakeBinaryExprV2(EBinaryOperatorV2::Divide, std::move(left), std::move(right));
    }

    inline auto PowExprV2(FExpressionPtrV2 base, FExpressionPtrV2 exponent) -> FExpressionPtrV2 {
        return MakeBinaryExprV2(EBinaryOperatorV2::Power, std::move(base), std::move(exponent));
    }

    inline auto MakeVocabularyEntryV2(Str entryId,
                                      Str symbol,
                                      Str preferredNotation,
                                      const EBaseVocabularyEntryKindV2 kind,
                                      Str summary,
                                      Str sourcePresetId,
                                      Str displayName = {},
                                      Str description = {}) -> FBaseVocabularyEntryV2 {
        FBaseVocabularyEntryV2 entry;
        entry.EntryId = std::move(entryId);
        entry.Symbol = std::move(symbol);
        entry.PreferredNotation = std::move(preferredNotation);
        entry.Kind = kind;
        entry.SemanticRoleSummary = std::move(summary);
        entry.SourcePresetId = std::move(sourcePresetId);
        entry.DisplayName = displayName.empty() ? entry.PreferredNotation : std::move(displayName);
        entry.Description = std::move(description);
        return entry;
    }

    inline auto RenderBaseVocabularyEntryLatexV2(const FBaseVocabularyEntryV2 &entry) -> Str {
        if (!entry.PreviewLatex.empty()) return entry.PreviewLatex;
        if (!entry.PreferredNotation.empty()) return entry.PreferredNotation;
        return entry.Symbol;
    }

    inline auto RenderBaseVocabularyEntryLabelV2(const FBaseVocabularyEntryV2 &entry) -> Str {
        if (!entry.DisplayName.empty()) return entry.DisplayName;
        if (!entry.PreferredNotation.empty()) return entry.PreferredNotation;
        if (!entry.Symbol.empty()) return entry.Symbol;
        return entry.EntryId;
    }

    inline auto BuildCoreMathVocabularyPresetV2() -> FBaseVocabularyPresetV2 {
        FBaseVocabularyPresetV2 preset;
        preset.PresetId = "core_math";
        preset.Name = "Core Math";
        preset.Description =
            "Ambient scalar sets, basic relation symbols, and authoring conventions shared by most models.";
        preset.Tags = {"math", "ambient", "core"};

        auto reals = MakeVocabularyEntryV2(
            "vocab.core_math.set.reals",
            "R",
            "\\mathbb{R}",
            EBaseVocabularyEntryKindV2::ScalarSet,
            "Scalar set of real numbers used as an ambient codomain/domain.",
            preset.PresetId,
            "Real Numbers",
            "Built-in real scalar set available without a local Definition.");
        reals.SourceText = "\\mathbb{R}";
        reals.Tags = {"set", "scalar"};
        preset.Entries.push_back(std::move(reals));

        auto complexes = MakeVocabularyEntryV2(
            "vocab.core_math.set.complexes",
            "C",
            "\\mathbb{C}",
            EBaseVocabularyEntryKindV2::ScalarSet,
            "Scalar set of complex numbers used as an ambient codomain/domain.",
            preset.PresetId,
            "Complex Numbers",
            "Built-in complex scalar set available without a local Definition.");
        complexes.SourceText = "\\mathbb{C}";
        complexes.Tags = {"set", "scalar"};
        preset.Entries.push_back(std::move(complexes));

        auto naturals = MakeVocabularyEntryV2(
            "vocab.core_math.set.naturals",
            "N",
            "\\mathbb{N}",
            EBaseVocabularyEntryKindV2::ScalarSet,
            "Scalar set of natural numbers available as ambient mathematical language.",
            preset.PresetId,
            "Natural Numbers",
            "Built-in natural-number set available without a local Definition.");
        naturals.SourceText = "\\mathbb{N}";
        naturals.Tags = {"set", "scalar"};
        preset.Entries.push_back(std::move(naturals));

        auto membership = MakeVocabularyEntryV2(
            "vocab.core_math.relation.membership",
            "in",
            "\\in",
            EBaseVocabularyEntryKindV2::RelationSymbol,
            "Ambient set-membership relation used in scalar declarations.",
            preset.PresetId,
            "Membership",
            "Membership relation used by definition notation such as x \\in \\mathbb{R}.");
        membership.SourceText = "\\in";
        membership.Tags = {"relation", "notation"};
        preset.Entries.push_back(std::move(membership));

        auto arrow = MakeVocabularyEntryV2(
            "vocab.core_math.relation.arrow",
            "to",
            "\\to",
            EBaseVocabularyEntryKindV2::FunctionConvention,
            "Ambient function-arrow convention used in type declarations.",
            preset.PresetId,
            "Function Arrow",
            "Function-type arrow used by authoring notation such as f : X \\to Y.");
        arrow.SourceText = "\\to";
        arrow.Tags = {"relation", "notation"};
        preset.Entries.push_back(std::move(arrow));

        auto equality = MakeVocabularyEntryV2(
            "vocab.core_math.relation.equality",
            "=",
            "=",
            EBaseVocabularyEntryKindV2::RelationSymbol,
            "Ambient equality relation used by model relations.",
            preset.PresetId,
            "Equality",
            "Equality relation separating left and right relation expressions.");
        equality.SourceText = "=";
        equality.Tags = {"relation", "notation"};
        preset.Entries.push_back(std::move(equality));

        auto multiplication = MakeVocabularyEntryV2(
            "vocab.core_math.arithmetic.multiplication",
            "*",
            "*",
            EBaseVocabularyEntryKindV2::ArithmeticConvention,
            "Ambient multiplication and implicit product convention.",
            preset.PresetId,
            "Multiplication",
            "Implicit and explicit multiplication conventions available in expression authoring.");
        multiplication.Tags = {"arithmetic", "notation"};
        preset.Entries.push_back(std::move(multiplication));

        auto power = MakeVocabularyEntryV2(
            "vocab.core_math.arithmetic.power",
            "^",
            "^",
            EBaseVocabularyEntryKindV2::ArithmeticConvention,
            "Ambient exponentiation convention.",
            preset.PresetId,
            "Exponentiation",
            "Power notation available in definitions and relations.");
        power.Tags = {"arithmetic", "notation"};
        preset.Entries.push_back(std::move(power));

        auto grouping = MakeVocabularyEntryV2(
            "vocab.core_math.notation.grouping",
            "()",
            "()",
            EBaseVocabularyEntryKindV2::NotationConvention,
            "Ambient grouping convention for expression authoring.",
            preset.PresetId,
            "Grouping",
            "Parentheses and grouping conventions used by the current notation layer.");
        grouping.Tags = {"notation"};
        preset.Entries.push_back(std::move(grouping));

        return preset;
    }

    inline auto BuildClassicalMechanicsVocabularyPresetV2() -> FBaseVocabularyPresetV2 {
        FBaseVocabularyPresetV2 preset;
        preset.PresetId = "classical_mechanics";
        preset.Name = "Classical Mechanics";
        preset.Description =
            "Core Math plus ambient time-derivative conventions for ODE-style authoring.";
        preset.ParentPresetId = "core_math";
        preset.Tags = {"mechanics", "ode", "ambient"};

        auto dot = MakeVocabularyEntryV2(
            "vocab.classical_mechanics.notation.dot",
            "dot",
            "\\dot",
            EBaseVocabularyEntryKindV2::NotationConvention,
            "Time-derivative notation convention for first ordinary derivatives.",
            preset.PresetId,
            "Dot Derivative",
            "First-order ordinary time-derivative convention for symbols such as \\dot x.");
        dot.PreviewLatex = "\\dot{\\mathrm{state}}";
        dot.SourceText = "\\dot";
        dot.Tags = {"derivative", "notation", "time"};
        preset.Entries.push_back(std::move(dot));

        auto ddot = MakeVocabularyEntryV2(
            "vocab.classical_mechanics.notation.ddot",
            "ddot",
            "\\ddot",
            EBaseVocabularyEntryKindV2::NotationConvention,
            "Time-derivative notation convention for second ordinary derivatives.",
            preset.PresetId,
            "Double-Dot Derivative",
            "Second-order ordinary time-derivative convention for symbols such as \\ddot x.");
        ddot.PreviewLatex = "\\ddot{\\mathrm{state}}";
        ddot.SourceText = "\\ddot";
        ddot.Tags = {"derivative", "notation", "time"};
        preset.Entries.push_back(std::move(ddot));

        return preset;
    }

    inline auto BuildRelativisticFieldTheoryVocabularyPresetV2() -> FBaseVocabularyPresetV2 {
        FBaseVocabularyPresetV2 preset;
        preset.PresetId = "relativistic_field_theory";
        preset.Name = "Relativistic Field Theory";
        preset.Description =
            "Core Math plus ambient operator vocabulary for Klein-Gordon-like authoring.";
        preset.ParentPresetId = "core_math";
        preset.Tags = {"field-theory", "pde", "ambient"};

        auto box = MakeVocabularyEntryV2(
            "vocab.relativistic_field_theory.operator.box",
            "Box",
            "\\Box",
            EBaseVocabularyEntryKindV2::Operator,
            "Ambient d'Alembert / wave-operator family symbol without fixing coordinates or metric realization.",
            preset.PresetId,
            "d'Alembertian",
            "Unary ambient operator placeholder for spacetime field equations such as \\Box \\phi = 0.");
        box.DefinitionKind = EDefinitionKindV2::OperatorSymbol;
        box.bGenericUnaryScalarFunctionOperator = true;
        box.PreviewLatex = "\\Box";
        box.SourceText = "\\Box";
        box.Tags = {"operator", "field-theory", "wave"};
        preset.Entries.push_back(std::move(box));

        auto partial = MakeVocabularyEntryV2(
            "vocab.relativistic_field_theory.operator.partial",
            "partial",
            "\\partial",
            EBaseVocabularyEntryKindV2::Operator,
            "Partial-derivative operator family placeholder for spacetime authoring.",
            preset.PresetId,
            "Partial Derivative",
            "Minimal ambient placeholder for partial-derivative notation. Indexed forms remain future work.");
        partial.DefinitionKind = EDefinitionKindV2::OperatorSymbol;
        partial.bGenericUnaryScalarFunctionOperator = true;
        partial.PreviewLatex = "\\partial_{\\mu}";
        partial.SourceText = "\\partial";
        partial.Tags = {"operator", "field-theory", "derivative"};
        preset.Entries.push_back(std::move(partial));

        auto spacetimeNotation = MakeVocabularyEntryV2(
            "vocab.relativistic_field_theory.notation.spacetime",
            "spacetime",
            "spacetime",
            EBaseVocabularyEntryKindV2::NotationConvention,
            "Ambient spacetime-oriented authoring convention without committing to coordinates or signature.",
            preset.PresetId,
            "Spacetime Convention",
            "Signals that indexed and spacetime-oriented notation belongs to the ambient language rather than local Definitions.");
        spacetimeNotation.Tags = {"notation", "field-theory", "spacetime"};
        preset.Entries.push_back(std::move(spacetimeNotation));

        return preset;
    }

    inline auto GetBaseVocabularyPresetCatalogV2() -> const Vector<FBaseVocabularyPresetV2> & {
        static const Vector<FBaseVocabularyPresetV2> Catalog = {
            BuildCoreMathVocabularyPresetV2(),
            BuildClassicalMechanicsVocabularyPresetV2(),
            BuildRelativisticFieldTheoryVocabularyPresetV2()
        };
        return Catalog;
    }

    inline auto FindBaseVocabularyPresetByIdV2(const Str &presetId) -> const FBaseVocabularyPresetV2 * {
        const auto &catalog = GetBaseVocabularyPresetCatalogV2();
        const auto it = std::find_if(catalog.begin(), catalog.end(), [&](const auto &preset) {
            return preset.PresetId == presetId;
        });
        if (it == catalog.end()) return nullptr;
        return &(*it);
    }

    inline auto ResolveBaseVocabularyPresetEntriesV2(const Str &presetId) -> Vector<FBaseVocabularyEntryV2> {
        const auto *preset = FindBaseVocabularyPresetByIdV2(presetId);
        if (preset == nullptr) return {};

        Vector<FBaseVocabularyEntryV2> entries;
        if (!preset->ParentPresetId.empty()) {
            entries = ResolveBaseVocabularyPresetEntriesV2(preset->ParentPresetId);
        }

        for (auto entry : preset->Entries) {
            if (entry.SourcePresetId.empty()) entry.SourcePresetId = preset->PresetId;
            const auto it = std::find_if(entries.begin(), entries.end(), [&](const auto &existing) {
                return existing.EntryId == entry.EntryId;
            });
            if (it == entries.end()) entries.push_back(std::move(entry));
            else *it = std::move(entry);
        }

        return entries;
    }

    inline auto FindDefinitionByIdV2(const FModelV2 &model, const Str &definitionId) -> const FDefinitionV2 * {
        const auto it = std::find_if(model.Definitions.begin(), model.Definitions.end(), [&](const auto &definition) {
            return definition.DefinitionId == definitionId;
        });
        if (it == model.Definitions.end()) return nullptr;
        return &(*it);
    }

    inline auto FindDefinitionByIdV2(FModelV2 &model, const Str &definitionId) -> FDefinitionV2 * {
        const auto it = std::find_if(model.Definitions.begin(), model.Definitions.end(), [&](const auto &definition) {
            return definition.DefinitionId == definitionId;
        });
        if (it == model.Definitions.end()) return nullptr;
        return &(*it);
    }

    inline auto FindDefinitionBySymbolV2(const FModelV2 &model, const Str &symbol) -> const FDefinitionV2 * {
        const auto it = std::find_if(model.Definitions.begin(), model.Definitions.end(), [&](const auto &definition) {
            return definition.Symbol == symbol || definition.PreferredNotation == symbol;
        });
        if (it == model.Definitions.end()) return nullptr;
        return &(*it);
    }

    inline auto FindMatchingLocalDefinitionForVocabularyEntryV2(const FModelV2 &model,
                                                                const FBaseVocabularyEntryV2 &entry) -> const FDefinitionV2 * {
        const auto normalizedEntrySymbol = NormalizeSymbolAliasTextV2(
            entry.PreferredNotation.empty() ? entry.Symbol : entry.PreferredNotation);
        const auto matchesExplicitEntryId = [&](const FDefinitionV2 &definition) {
            const auto it = definition.Metadata.find("base_vocabulary_entry_id");
            return it != definition.Metadata.end() && it->second == entry.EntryId;
        };
        const auto matchesAlias = [&](const FDefinitionV2 &definition) {
            return NormalizeSymbolAliasTextV2(definition.Symbol) == normalizedEntrySymbol ||
                NormalizeSymbolAliasTextV2(definition.PreferredNotation) == normalizedEntrySymbol;
        };

        const auto explicitIt = std::find_if(model.Definitions.begin(), model.Definitions.end(), matchesExplicitEntryId);
        if (explicitIt != model.Definitions.end()) return &(*explicitIt);

        const auto aliasIt = std::find_if(model.Definitions.begin(), model.Definitions.end(), matchesAlias);
        if (aliasIt == model.Definitions.end()) return nullptr;
        return &(*aliasIt);
    }

    inline auto ClassifyVocabularyOverrideStatusV2(const FBaseVocabularyEntryV2 &entry,
                                                   const FDefinitionV2 *definition) -> EVocabularyOverrideStatusV2 {
        if (definition == nullptr) return EVocabularyOverrideStatusV2::BuiltIn;

        if (const auto it = definition->Metadata.find("base_vocabulary_override_mode");
            it != definition->Metadata.end()) {
            if (it->second == "override") return EVocabularyOverrideStatusV2::OverriddenLocally;
            if (it->second == "specialize") return EVocabularyOverrideStatusV2::SpecializedLocally;
        }

        if (entry.DefinitionKind.has_value() && definition->Kind == *entry.DefinitionKind) {
            return EVocabularyOverrideStatusV2::SpecializedLocally;
        }

        return EVocabularyOverrideStatusV2::OverriddenLocally;
    }

    inline auto ResolveModelBaseVocabularyEntriesV2(const FModelV2 &model) -> Vector<FResolvedBaseVocabularyEntryV2> {
        Vector<FResolvedBaseVocabularyEntryV2> resolved;
        const auto presetEntries = ResolveBaseVocabularyPresetEntriesV2(model.BaseVocabulary.ActivePresetId);
        resolved.reserve(presetEntries.size());

        for (const auto &entry : presetEntries) {
            FResolvedBaseVocabularyEntryV2 resolvedEntry;
            resolvedEntry.Entry = entry;
            if (const auto *definition = FindMatchingLocalDefinitionForVocabularyEntryV2(model, entry); definition != nullptr) {
                resolvedEntry.LocalDefinitionId = definition->DefinitionId;
                resolvedEntry.OverrideStatus = ClassifyVocabularyOverrideStatusV2(entry, definition);
            }
            resolved.push_back(std::move(resolvedEntry));
        }

        return resolved;
    }

    inline auto FindBaseVocabularyEntryByIdV2(const FModelV2 &model, const Str &entryId) -> TOptional<FBaseVocabularyEntryV2> {
        const auto entries = ResolveBaseVocabularyPresetEntriesV2(model.BaseVocabulary.ActivePresetId);
        const auto it = std::find_if(entries.begin(), entries.end(), [&](const auto &entry) {
            return entry.EntryId == entryId;
        });
        if (it == entries.end()) return std::nullopt;
        return *it;
    }

    inline auto FindBaseVocabularyEntryByAliasV2(const FModelV2 &model, const Str &alias) -> TOptional<FBaseVocabularyEntryV2> {
        const auto normalizedAlias = NormalizeSymbolAliasTextV2(alias);
        if (normalizedAlias.empty()) return std::nullopt;

        const auto entries = ResolveBaseVocabularyPresetEntriesV2(model.BaseVocabulary.ActivePresetId);
        const auto it = std::find_if(entries.begin(), entries.end(), [&](const auto &entry) {
            return NormalizeSymbolAliasTextV2(entry.Symbol) == normalizedAlias ||
                NormalizeSymbolAliasTextV2(entry.PreferredNotation) == normalizedAlias;
        });
        if (it == entries.end()) return std::nullopt;
        return *it;
    }

    inline auto FindBaseVocabularyReferenceByAliasV2(const FModelV2 &model, const Str &alias) -> TOptional<FReferenceV2> {
        if (const auto entry = FindBaseVocabularyEntryByAliasV2(model, alias); entry.has_value()) {
            return MakeReferenceV2(entry->EntryId, alias);
        }
        return std::nullopt;
    }

    struct FResolvedSemanticEntryV2 {
        ESemanticOriginV2 Origin = ESemanticOriginV2::Unresolved;
        const FDefinitionV2 *Definition = nullptr;
        TOptional<FBaseVocabularyEntryV2> VocabularyEntry;

        [[nodiscard]] auto IsResolved() const -> bool {
            return Definition != nullptr || VocabularyEntry.has_value();
        }

        [[nodiscard]] auto GetDeclaredKind() const -> TOptional<EDefinitionKindV2> {
            if (Definition != nullptr) return Definition->Kind;
            if (VocabularyEntry.has_value()) return VocabularyEntry->DefinitionKind;
            return std::nullopt;
        }

        [[nodiscard]] auto GetDeclaredType() const -> const FTypeExprV2 * {
            if (Definition != nullptr && Definition->DeclaredType.has_value()) return &(*Definition->DeclaredType);
            if (VocabularyEntry.has_value() && VocabularyEntry->DeclaredType.has_value()) return &(*VocabularyEntry->DeclaredType);
            return nullptr;
        }

        [[nodiscard]] auto GetOperatorStyle() const -> EOperatorApplicationStyleV2 {
            if (Definition != nullptr) return Definition->OperatorStyle;
            if (VocabularyEntry.has_value()) return VocabularyEntry->OperatorStyle;
            return EOperatorApplicationStyleV2::Prefix;
        }

        [[nodiscard]] auto IsGenericUnaryScalarFunctionOperator() const -> bool {
            return VocabularyEntry.has_value() && VocabularyEntry->bGenericUnaryScalarFunctionOperator;
        }

        [[nodiscard]] auto GetReferenceId() const -> Str {
            if (Definition != nullptr) return Definition->DefinitionId;
            if (VocabularyEntry.has_value()) return VocabularyEntry->EntryId;
            return {};
        }

        [[nodiscard]] auto GetDisplayLabel() const -> Str {
            if (Definition != nullptr) {
                if (!Definition->PreferredNotation.empty()) return Definition->PreferredNotation;
                if (!Definition->Symbol.empty()) return Definition->Symbol;
                return Definition->DefinitionId;
            }
            if (VocabularyEntry.has_value()) {
                if (!VocabularyEntry->PreferredNotation.empty()) return VocabularyEntry->PreferredNotation;
                if (!VocabularyEntry->Symbol.empty()) return VocabularyEntry->Symbol;
                return VocabularyEntry->EntryId;
            }
            return {};
        }
    };

    inline auto ResolveSemanticEntryV2(const FModelV2 &model, const FReferenceV2 &reference) -> FResolvedSemanticEntryV2 {
        if (!reference.IsBound()) return {};

        if (const auto *definition = FindDefinitionByIdV2(model, reference.ReferenceId); definition != nullptr) {
            return FResolvedSemanticEntryV2{
                .Origin = ESemanticOriginV2::LocalDefinition,
                .Definition = definition
            };
        }

        if (const auto entry = FindBaseVocabularyEntryByIdV2(model, reference.ReferenceId); entry.has_value()) {
            return FResolvedSemanticEntryV2{
                .Origin = ESemanticOriginV2::BaseVocabulary,
                .VocabularyEntry = entry
            };
        }

        return {};
    }

    inline auto FindRelationByIdV2(const FModelV2 &model, const Str &relationId) -> const FRelationV2 * {
        const auto it = std::find_if(model.Relations.begin(), model.Relations.end(), [&](const auto &relation) {
            return relation.RelationId == relationId;
        });
        if (it == model.Relations.end()) return nullptr;
        return &(*it);
    }

    inline auto FindRelationByIdV2(FModelV2 &model, const Str &relationId) -> FRelationV2 * {
        const auto it = std::find_if(model.Relations.begin(), model.Relations.end(), [&](const auto &relation) {
            return relation.RelationId == relationId;
        });
        if (it == model.Relations.end()) return nullptr;
        return &(*it);
    }

    inline auto FindAssumptionStateByIdV2(const FModelV2 &model, const Str &assumptionId) -> const FAssumptionStateV2 * {
        const auto it = std::find_if(model.AssumptionStates.begin(), model.AssumptionStates.end(), [&](const auto &state) {
            return state.AssumptionId == assumptionId;
        });
        if (it == model.AssumptionStates.end()) return nullptr;
        return &(*it);
    }

    inline auto FindAssumptionStateByIdV2(FModelV2 &model, const Str &assumptionId) -> FAssumptionStateV2 * {
        const auto it = std::find_if(model.AssumptionStates.begin(), model.AssumptionStates.end(), [&](const auto &state) {
            return state.AssumptionId == assumptionId;
        });
        if (it == model.AssumptionStates.end()) return nullptr;
        return &(*it);
    }

    template<typename FVisitor>
    inline auto VisitExpressionDepthFirstV2(const FExpressionPtrV2 &expression, const FVisitor &visitor) -> void {
        if (expression == nullptr) return;
        visitor(*expression);
        for (const auto &child : expression->Children) {
            VisitExpressionDepthFirstV2(child, visitor);
        }
    }

    inline auto IsZeroLiteralV2(const FExpressionPtrV2 &expression) -> bool {
        if (expression == nullptr || expression->Kind != EExpressionKindV2::Literal) return false;
        return TrimAsciiCopyV2(expression->LiteralText) == "0";
    }

    inline auto RenderExpressionCompactV2(const FExpressionPtrV2 &expression) -> Str;

    inline auto RenderSpaceExprV2(const FSpaceExprV2 &space) -> Str {
        Str base = space.Name;
        if (base.empty()) {
            if (space.Kind == ESpaceKindV2::RealNumbers) base = "R";
            else if (space.Kind == ESpaceKindV2::ComplexNumbers) base = "C";
            else base = "Space";
        }

        if (!space.Dimension.has_value() || *space.Dimension == nullptr) return base;
        return base + "^(" + RenderExpressionCompactV2(*space.Dimension) + ")";
    }

    inline auto RenderTypeExprV2(const FTypeExprV2 &type) -> Str {
        if (type.Kind == ETypeExprKindV2::Space) return RenderSpaceExprV2(type.Space);

        Slab::StringStream ss;
        for (std::size_t i = 0; i < type.Domain.size(); ++i) {
            if (i != 0) ss << " x ";
            ss << RenderTypeExprV2(type.Domain[i]);
        }
        ss << " -> ";
        if (type.Codomain != nullptr) ss << RenderTypeExprV2(*type.Codomain);
        else ss << "?";
        return ss.str();
    }

    inline auto RenderExpressionCompactV2(const FExpressionPtrV2 &expression) -> Str {
        if (expression == nullptr) return "<null>";

        switch (expression->Kind) {
            case EExpressionKindV2::Symbol:
                return expression->Reference.DisplayText();
            case EExpressionKindV2::Literal:
                return expression->LiteralText;
            case EExpressionKindV2::Unary:
                if (expression->Children.empty()) return Str(ToString(expression->UnaryOperator)) + "?";
                return Str(ToString(expression->UnaryOperator)) + RenderExpressionCompactV2(expression->Children.front());
            case EExpressionKindV2::Binary: {
                if (expression->Children.size() < 2) return "?";
                const auto left = RenderExpressionCompactV2(expression->Children[0]);
                const auto right = RenderExpressionCompactV2(expression->Children[1]);
                return "(" + left + " " + ToString(expression->BinaryOperator) + " " + right + ")";
            }
            case EExpressionKindV2::FunctionApplication: {
                Slab::StringStream ss;
                ss << expression->Reference.DisplayText() << "(";
                for (std::size_t i = 0; i < expression->Children.size(); ++i) {
                    if (i != 0) ss << ", ";
                    ss << RenderExpressionCompactV2(expression->Children[i]);
                }
                ss << ")";
                return ss.str();
            }
            case EExpressionKindV2::OperatorApplication: {
                Slab::StringStream ss;
                ss << expression->Reference.DisplayText();
                if (!expression->Children.empty()) ss << " ";
                for (std::size_t i = 0; i < expression->Children.size(); ++i) {
                    if (i != 0) ss << ", ";
                    ss << RenderExpressionCompactV2(expression->Children[i]);
                }
                return ss.str();
            }
            case EExpressionKindV2::Derivative: {
                if (expression->Children.empty()) return "d(?)";
                Slab::StringStream ss;
                ss << ToString(expression->DerivativeFlavor) << "Derivative[";
                for (std::size_t i = 0; i < expression->DerivativeVariables.size(); ++i) {
                    if (i != 0) ss << ", ";
                    ss << expression->DerivativeVariables[i].DisplayText();
                }
                ss << "](" << RenderExpressionCompactV2(expression->Children.front()) << ")";
                return ss.str();
            }
        }

        return "?";
    }

    inline auto RenderDefinitionLabelV2(const FDefinitionV2 &definition) -> Str {
        if (!definition.PreferredNotation.empty()) return definition.PreferredNotation;
        if (!definition.Symbol.empty()) return definition.Symbol;
        return definition.DefinitionId;
    }

    inline auto RenderDefinitionSummaryV2(const FDefinitionV2 &definition) -> Str {
        Slab::StringStream ss;
        ss << RenderDefinitionLabelV2(definition) << " [" << ToString(definition.Kind) << "]";
        if (definition.DeclaredType.has_value()) {
            ss << " : " << RenderTypeExprV2(*definition.DeclaredType);
        }
        return ss.str();
    }

    inline auto RenderRelationSummaryV2(const FRelationV2 &relation) -> Str {
        return RenderExpressionCompactV2(relation.Left) + " = " + RenderExpressionCompactV2(relation.Right);
    }

    inline auto IsScalarSpaceExprV2(const FSpaceExprV2 &space) -> bool {
        if (space.Kind == ESpaceKindV2::NamedSpace) return false;
        return !space.Dimension.has_value() || *space.Dimension == nullptr;
    }

    inline auto IsScalarTypeV2(const FTypeExprV2 &type) -> bool {
        if (type.Kind == ETypeExprKindV2::Space) return IsScalarSpaceExprV2(type.Space);
        if (type.Codomain == nullptr) return false;
        return IsScalarTypeV2(*type.Codomain);
    }

    inline auto AreSpaceExprEqualV2(const FSpaceExprV2 &lhs, const FSpaceExprV2 &rhs) -> bool {
        if (lhs.Kind != rhs.Kind) return false;
        if (lhs.Name != rhs.Name) return false;

        const auto lhsHasDim = lhs.Dimension.has_value() && *lhs.Dimension != nullptr;
        const auto rhsHasDim = rhs.Dimension.has_value() && *rhs.Dimension != nullptr;
        if (lhsHasDim != rhsHasDim) return false;
        if (!lhsHasDim) return true;

        return RenderExpressionCompactV2(*lhs.Dimension) == RenderExpressionCompactV2(*rhs.Dimension);
    }

    inline auto AreTypesEquivalentV2(const FTypeExprV2 &lhs, const FTypeExprV2 &rhs) -> bool {
        if (lhs.Kind != rhs.Kind) return false;

        if (lhs.Kind == ETypeExprKindV2::Space) return AreSpaceExprEqualV2(lhs.Space, rhs.Space);

        if (lhs.Domain.size() != rhs.Domain.size()) return false;
        if ((lhs.Codomain == nullptr) != (rhs.Codomain == nullptr)) return false;

        for (std::size_t i = 0; i < lhs.Domain.size(); ++i) {
            if (!AreTypesEquivalentV2(lhs.Domain[i], rhs.Domain[i])) return false;
        }

        if (lhs.Codomain == nullptr) return true;
        return AreTypesEquivalentV2(*lhs.Codomain, *rhs.Codomain);
    }

    inline auto IsDifferentiableDefinitionKindV2(const EDefinitionKindV2 kind) -> bool {
        return kind == EDefinitionKindV2::Coordinate ||
               kind == EDefinitionKindV2::StateVariable ||
               kind == EDefinitionKindV2::Field ||
               kind == EDefinitionKindV2::ObservableSymbol;
    }

    inline auto ExpressionSupportsDifferentiationV2(const FModelV2 &model,
                                                    const FExpressionPtrV2 &expression) -> bool {
        if (expression == nullptr) return false;

        switch (expression->Kind) {
            case EExpressionKindV2::Literal:
                return false;
            case EExpressionKindV2::Symbol: {
                if (!expression->Reference.IsBound()) return !expression->Reference.SymbolText.empty();
                const auto resolved = ResolveSemanticEntryV2(model, expression->Reference);
                if (resolved.Definition != nullptr) return IsDifferentiableDefinitionKindV2(resolved.Definition->Kind);
                if (const auto kind = resolved.GetDeclaredKind(); kind.has_value()) return IsDifferentiableDefinitionKindV2(*kind);
                return false;
            }
            case EExpressionKindV2::FunctionApplication:
            case EExpressionKindV2::OperatorApplication:
            case EExpressionKindV2::Derivative:
                return true;
            case EExpressionKindV2::Unary:
            case EExpressionKindV2::Binary:
                return std::any_of(expression->Children.begin(), expression->Children.end(), [&](const auto &child) {
                    return ExpressionSupportsDifferentiationV2(model, child);
                });
        }

        return false;
    }

    inline auto InferExpressionTypeV2(const FModelV2 &model,
                                      const FExpressionPtrV2 &expression,
                                      Str *pError = nullptr) -> TOptional<FTypeExprV2> {
        const auto fail = [&](const Str &message) -> TOptional<FTypeExprV2> {
            if (pError != nullptr) *pError = message;
            return std::nullopt;
        };

        if (expression == nullptr) return fail("expression is null");

        switch (expression->Kind) {
            case EExpressionKindV2::Literal: {
                auto text = TrimAsciiCopyV2(expression->LiteralText);
                if (text.empty()) return fail("literal text is empty");
                return MakeSpaceTypeV2(MakeRealSpaceV2());
            }
            case EExpressionKindV2::Symbol: {
                if (!expression->Reference.IsBound()) {
                    return fail("symbol '" + expression->Reference.DisplayText() + "' is unresolved");
                }

                const auto resolved = ResolveSemanticEntryV2(model, expression->Reference);
                if (!resolved.IsResolved()) {
                    return fail("symbol '" + expression->Reference.ReferenceId + "' is undefined");
                }

                const auto *declaredType = resolved.GetDeclaredType();
                if (declaredType == nullptr) {
                    return fail("definition '" + resolved.GetReferenceId() + "' has no declared type");
                }
                return *declaredType;
            }
            case EExpressionKindV2::Unary: {
                if (expression->Children.size() != 1) return fail("unary expression must have one child");
                return InferExpressionTypeV2(model, expression->Children.front(), pError);
            }
            case EExpressionKindV2::Binary: {
                if (expression->Children.size() != 2) return fail("binary expression must have two children");

                Str leftError;
                Str rightError;
                const auto leftType = InferExpressionTypeV2(model, expression->Children[0], &leftError);
                const auto rightType = InferExpressionTypeV2(model, expression->Children[1], &rightError);
                if (!leftType.has_value()) return fail(leftError);
                if (!rightType.has_value()) return fail(rightError);

                if (expression->BinaryOperator == EBinaryOperatorV2::Add ||
                    expression->BinaryOperator == EBinaryOperatorV2::Subtract) {
                    if (AreTypesEquivalentV2(*leftType, *rightType)) return leftType;
                    if (IsZeroLiteralV2(expression->Children[0]) && IsScalarTypeV2(*rightType)) return rightType;
                    if (IsZeroLiteralV2(expression->Children[1]) && IsScalarTypeV2(*leftType)) return leftType;
                    return fail("additive operands have incompatible types '" +
                                RenderTypeExprV2(*leftType) + "' and '" + RenderTypeExprV2(*rightType) + "'");
                }

                if (expression->BinaryOperator == EBinaryOperatorV2::Multiply ||
                    expression->BinaryOperator == EBinaryOperatorV2::Divide) {
                    if (leftType->Kind == ETypeExprKindV2::Space && rightType->Kind == ETypeExprKindV2::Space &&
                        IsScalarTypeV2(*leftType) && IsScalarTypeV2(*rightType)) {
                        return leftType;
                    }

                    if (leftType->Kind == ETypeExprKindV2::Space &&
                        rightType->Kind == ETypeExprKindV2::Function &&
                        IsScalarTypeV2(*leftType) &&
                        IsScalarTypeV2(*rightType)) {
                        return rightType;
                    }

                    if (leftType->Kind == ETypeExprKindV2::Function &&
                        rightType->Kind == ETypeExprKindV2::Space &&
                        IsScalarTypeV2(*leftType) &&
                        IsScalarTypeV2(*rightType)) {
                        return leftType;
                    }

                    if (leftType->Kind == ETypeExprKindV2::Function &&
                        rightType->Kind == ETypeExprKindV2::Function &&
                        IsScalarTypeV2(*leftType) &&
                        IsScalarTypeV2(*rightType) &&
                        AreTypesEquivalentV2(*leftType, *rightType)) {
                        return leftType;
                    }

                    return fail("multiplicative operands have incompatible types '" +
                                RenderTypeExprV2(*leftType) + "' and '" + RenderTypeExprV2(*rightType) + "'");
                }

                if (expression->BinaryOperator == EBinaryOperatorV2::Power) {
                    if (!IsScalarTypeV2(*rightType)) {
                        return fail("power exponent must be scalar");
                    }

                    if (leftType->Kind == ETypeExprKindV2::Space && IsScalarTypeV2(*leftType)) return leftType;
                    if (leftType->Kind == ETypeExprKindV2::Function && IsScalarTypeV2(*leftType)) return leftType;
                    return fail("power base must be scalar or scalar-valued function");
                }

                return fail("unsupported binary operator");
            }
            case EExpressionKindV2::FunctionApplication:
            case EExpressionKindV2::OperatorApplication: {
                if (!expression->Reference.IsBound()) {
                    return fail("callee '" + expression->Reference.DisplayText() + "' is unresolved");
                }

                const auto resolved = ResolveSemanticEntryV2(model, expression->Reference);
                if (!resolved.IsResolved()) {
                    return fail("callee '" + expression->Reference.ReferenceId + "' is undefined");
                }

                const auto *declaredType = resolved.GetDeclaredType();
                if (declaredType == nullptr) {
                    if (resolved.IsGenericUnaryScalarFunctionOperator() && expression->Children.size() == 1) {
                        Str argumentError;
                        const auto argumentType = InferExpressionTypeV2(model, expression->Children.front(), &argumentError);
                        if (!argumentType.has_value()) return fail("argument[0] for '" + resolved.GetReferenceId() + "': " + argumentError);
                        if (argumentType->Kind != ETypeExprKindV2::Function || !IsScalarTypeV2(*argumentType)) {
                            return fail("argument[0] for '" + resolved.GetReferenceId() +
                                        "' must be a scalar-valued function-like expression");
                        }
                        return argumentType;
                    }
                    return fail("callee '" + resolved.GetReferenceId() + "' has no declared type");
                }
                if (declaredType->Kind != ETypeExprKindV2::Function || declaredType->Codomain == nullptr) {
                    return fail("callee '" + resolved.GetReferenceId() + "' is not callable");
                }

                const auto &signature = *declaredType;
                if (signature.Domain.size() != expression->Children.size()) {
                    return fail("callee '" + resolved.GetReferenceId() + "' expects " +
                                ToStr(signature.Domain.size()) + " arguments, got " +
                                ToStr(expression->Children.size()));
                }

                for (std::size_t i = 0; i < signature.Domain.size(); ++i) {
                    Str argumentError;
                    const auto argumentType = InferExpressionTypeV2(model, expression->Children[i], &argumentError);
                    if (!argumentType.has_value()) {
                        return fail("argument[" + ToStr(i) + "] for '" + resolved.GetReferenceId() + "': " + argumentError);
                    }
                    if (!AreTypesEquivalentV2(signature.Domain[i], *argumentType)) {
                        return fail("argument[" + ToStr(i) + "] for '" + resolved.GetReferenceId() +
                                    "' has type '" + RenderTypeExprV2(*argumentType) +
                                    "', expected '" + RenderTypeExprV2(signature.Domain[i]) + "'");
                    }
                }

                return *signature.Codomain;
            }
            case EExpressionKindV2::Derivative: {
                if (expression->Children.size() != 1) return fail("derivative must have one target expression");
                if (expression->DerivativeVariables.empty()) return fail("derivative has no variables");

                for (const auto &variable : expression->DerivativeVariables) {
                    if (!variable.IsBound()) {
                        return fail("derivative variable '" + variable.DisplayText() + "' is unresolved");
                    }
                    const auto *definition = FindDefinitionByIdV2(model, variable.ReferenceId);
                    if (definition == nullptr) return fail("derivative variable '" + variable.ReferenceId + "' is undefined");
                    if (definition->Kind != EDefinitionKindV2::Coordinate) {
                        return fail("derivative variable '" + definition->DefinitionId + "' is not a coordinate");
                    }
                }

                Str targetError;
                const auto targetType = InferExpressionTypeV2(model, expression->Children.front(), &targetError);
                if (!targetType.has_value()) return fail(targetError);

                if (!ExpressionSupportsDifferentiationV2(model, expression->Children.front())) {
                    return fail("derivative target is not differentiable");
                }

                return targetType;
            }
        }

        return fail("unsupported expression kind");
    }

    inline auto ValidateDefinitionTypeShapeV2(const FDefinitionV2 &definition,
                                              FValidationResultV2 &result) -> void {
        const auto addError = [&](const Str &message) {
            result.Add(EValidationSeverityV2::Error, definition.DefinitionId, "Definition", message);
        };

        if (definition.DefinitionId.empty()) addError("definition id is empty");
        if (definition.Symbol.empty()) addError("symbol is empty");

        if (!definition.DeclaredType.has_value()) {
            addError("declared type is missing");
            return;
        }

        const auto &type = *definition.DeclaredType;

        switch (definition.Kind) {
            case EDefinitionKindV2::ScalarParameter:
            case EDefinitionKindV2::Coordinate:
                if (type.Kind != ETypeExprKindV2::Space) {
                    addError("scalar/coordinate definitions must declare a space type");
                }
                break;
            case EDefinitionKindV2::StateVariable:
            case EDefinitionKindV2::Field:
            case EDefinitionKindV2::OperatorSymbol:
            case EDefinitionKindV2::ObservableSymbol:
                if (type.Kind != ETypeExprKindV2::Function || type.Codomain == nullptr) {
                    addError("function-like definitions must declare a function type with codomain");
                    return;
                }
                if (definition.Kind != EDefinitionKindV2::ObservableSymbol && type.Domain.empty()) {
                    addError("function-like definitions must declare at least one domain factor");
                }
                break;
        }
    }

    inline auto ValidateModelV2(const FModelV2 &model) -> FValidationResultV2 {
        FValidationResultV2 result;

        if (model.ModelId.empty()) {
            result.Add(EValidationSeverityV2::Error, {}, "Model", "model id is empty");
        }
        if (model.Name.empty()) {
            result.Add(EValidationSeverityV2::Error, model.ModelId, "Model", "model name is empty");
        }

        std::set<Str> definitionIds;
        std::set<Str> symbols;
        for (const auto &definition : model.Definitions) {
            ValidateDefinitionTypeShapeV2(definition, result);

            if (!definition.DefinitionId.empty() && !definitionIds.insert(definition.DefinitionId).second) {
                result.Add(
                    EValidationSeverityV2::Error,
                    definition.DefinitionId,
                    "Definition",
                    "duplicate definition id");
            }

            if (!definition.Symbol.empty() && !symbols.insert(definition.Symbol).second) {
                result.Add(
                    EValidationSeverityV2::Error,
                    definition.DefinitionId,
                    "Definition",
                    "duplicate definition symbol '" + definition.Symbol + "'");
            }

            for (const auto &argumentId : definition.ArgumentDefinitionIds) {
                if (argumentId.empty()) {
                    result.Add(EValidationSeverityV2::Error, definition.DefinitionId, "Definition", "argument reference is empty");
                    continue;
                }
                if (FindDefinitionByIdV2(model, argumentId) == nullptr) {
                    result.Add(
                        EValidationSeverityV2::Error,
                        definition.DefinitionId,
                        "Definition",
                        "argument reference '" + argumentId + "' is undefined");
                    continue;
                }

                if (definition.Kind == EDefinitionKindV2::StateVariable || definition.Kind == EDefinitionKindV2::Field) {
                    const auto *argumentDefinition = FindDefinitionByIdV2(model, argumentId);
                    if (argumentDefinition != nullptr && argumentDefinition->Kind != EDefinitionKindV2::Coordinate) {
                        result.Add(
                            EValidationSeverityV2::Error,
                            definition.DefinitionId,
                            "Definition",
                            "argument reference '" + argumentId + "' is not a coordinate");
                    }
                }
            }

            for (const auto &dependencyId : definition.DependencyDefinitionIds) {
                if (dependencyId.empty()) {
                    result.Add(EValidationSeverityV2::Error, definition.DefinitionId, "Definition", "dependency reference is empty");
                    continue;
                }
                if (FindDefinitionByIdV2(model, dependencyId) == nullptr) {
                    result.Add(
                        EValidationSeverityV2::Error,
                        definition.DefinitionId,
                        "Definition",
                        "dependency reference '" + dependencyId + "' is undefined");
                }
            }

            if (definition.ArgumentDefinitionIds.end() !=
                std::find(definition.ArgumentDefinitionIds.begin(),
                          definition.ArgumentDefinitionIds.end(),
                          definition.DefinitionId)) {
                result.Add(
                    EValidationSeverityV2::Error,
                    definition.DefinitionId,
                    "Definition",
                    "definition references itself as an argument");
            }

            if (definition.DependencyDefinitionIds.end() !=
                std::find(definition.DependencyDefinitionIds.begin(),
                          definition.DependencyDefinitionIds.end(),
                          definition.DefinitionId)) {
                result.Add(
                    EValidationSeverityV2::Error,
                    definition.DefinitionId,
                    "Definition",
                    "definition references itself as a dependency");
            }

            if (definition.DeclaredType.has_value() &&
                definition.DeclaredType->Kind == ETypeExprKindV2::Function &&
                definition.DeclaredType->Domain.size() != definition.ArgumentDefinitionIds.size() &&
                (definition.Kind == EDefinitionKindV2::StateVariable || definition.Kind == EDefinitionKindV2::Field) &&
                !definition.ArgumentDefinitionIds.empty()) {
                result.Add(
                    EValidationSeverityV2::Error,
                    definition.DefinitionId,
                    "Definition",
                    "argument reference count does not match declared domain arity");
            }
        }

        std::set<Str> relationIds;
        for (const auto &relation : model.Relations) {
            if (relation.RelationId.empty()) {
                result.Add(EValidationSeverityV2::Error, {}, "Relation", "relation id is empty");
            } else if (!relationIds.insert(relation.RelationId).second) {
                result.Add(EValidationSeverityV2::Error, relation.RelationId, "Relation", "duplicate relation id");
            }

            if (relation.Left == nullptr || relation.Right == nullptr) {
                result.Add(EValidationSeverityV2::Error, relation.RelationId, "Relation", "relation side is null");
                continue;
            }

            const auto validateExpression = [&](const FExpressionPtrV2 &expression, const char *sideLabel) {
                VisitExpressionDepthFirstV2(expression, [&](const FExpressionV2 &node) {
                    if (node.Kind == EExpressionKindV2::Symbol ||
                        node.Kind == EExpressionKindV2::FunctionApplication ||
                        node.Kind == EExpressionKindV2::OperatorApplication) {
                        if (!node.Reference.IsBound()) {
                            result.Add(
                                EValidationSeverityV2::Error,
                                relation.RelationId,
                                sideLabel,
                                Str("reference '") + node.Reference.DisplayText() + "' is unresolved");
                            return;
                        }
                        const auto resolved = ResolveSemanticEntryV2(model, node.Reference);
                        if (!resolved.IsResolved()) {
                            result.Add(
                                EValidationSeverityV2::Error,
                                relation.RelationId,
                                sideLabel,
                                Str("reference '") + node.Reference.ReferenceId + "' is undefined");
                            return;
                        }
                        const auto declaredKind = resolved.GetDeclaredKind();
                        if (node.Kind == EExpressionKindV2::FunctionApplication &&
                            !(declaredKind.has_value() &&
                              (*declaredKind == EDefinitionKindV2::StateVariable ||
                               *declaredKind == EDefinitionKindV2::Field ||
                               *declaredKind == EDefinitionKindV2::ObservableSymbol))) {
                            result.Add(
                                EValidationSeverityV2::Error,
                                relation.RelationId,
                                sideLabel,
                                Str("function application uses non-function definition '") + resolved.GetReferenceId() + "'");
                        }
                        if (node.Kind == EExpressionKindV2::OperatorApplication &&
                            !(declaredKind.has_value() && *declaredKind == EDefinitionKindV2::OperatorSymbol)) {
                            result.Add(
                                EValidationSeverityV2::Error,
                                relation.RelationId,
                                sideLabel,
                                Str("operator application uses non-operator definition '") + resolved.GetReferenceId() + "'");
                        }
                    }

                    if (node.Kind == EExpressionKindV2::Derivative) {
                        if (node.DerivativeVariables.empty()) {
                            result.Add(
                                EValidationSeverityV2::Error,
                                relation.RelationId,
                                sideLabel,
                                "derivative expression has no variables");
                        }

                        for (const auto &variable : node.DerivativeVariables) {
                            if (!variable.IsBound()) {
                                result.Add(
                                    EValidationSeverityV2::Error,
                                    relation.RelationId,
                                    sideLabel,
                                    Str("derivative variable '") + variable.DisplayText() + "' is unresolved");
                                continue;
                            }
                            const auto *variableDefinition = FindDefinitionByIdV2(model, variable.ReferenceId);
                            if (variableDefinition == nullptr) {
                                result.Add(
                                    EValidationSeverityV2::Error,
                                    relation.RelationId,
                                    sideLabel,
                                    Str("derivative variable '") + variable.ReferenceId + "' is undefined");
                                continue;
                            }
                            if (variableDefinition->Kind != EDefinitionKindV2::Coordinate) {
                                result.Add(
                                    EValidationSeverityV2::Error,
                                    relation.RelationId,
                                    sideLabel,
                                    Str("derivative variable '") + variableDefinition->DefinitionId + "' is not a coordinate");
                            }
                        }
                    }
                });
            };

            validateExpression(relation.Left, "Relation.Left");
            validateExpression(relation.Right, "Relation.Right");

            Str leftTypeError;
            Str rightTypeError;
            const auto leftType = InferExpressionTypeV2(model, relation.Left, &leftTypeError);
            const auto rightType = InferExpressionTypeV2(model, relation.Right, &rightTypeError);
            if (!leftType.has_value()) {
                result.Add(EValidationSeverityV2::Error, relation.RelationId, "Relation.Left", leftTypeError);
            }
            if (!rightType.has_value()) {
                result.Add(EValidationSeverityV2::Error, relation.RelationId, "Relation.Right", rightTypeError);
            }

            if (leftType.has_value() && rightType.has_value() &&
                !AreTypesEquivalentV2(*leftType, *rightType) &&
                !(IsZeroLiteralV2(relation.Left) && IsScalarTypeV2(*rightType)) &&
                !(IsZeroLiteralV2(relation.Right) && IsScalarTypeV2(*leftType))) {
                result.Add(
                    EValidationSeverityV2::Error,
                    relation.RelationId,
                    "Relation",
                    "relation sides have incompatible types '" +
                    RenderTypeExprV2(*leftType) + "' and '" + RenderTypeExprV2(*rightType) + "'");
            }
        }

        if (model.InitialConditions.has_value()) {
            const auto &initialConditions = *model.InitialConditions;
            const auto addInitialConditionError = [&](const Str &entityId,
                                                     const Str &context,
                                                     const Str &message) {
                result.Add(EValidationSeverityV2::Error, entityId, context, message);
            };

            if (initialConditions.TimeExpression == nullptr) {
                addInitialConditionError("initial_conditions", "InitialConditions", "initial-condition time expression is missing");
            } else {
                Str timeTypeError;
                const auto timeType = InferExpressionTypeV2(model, initialConditions.TimeExpression, &timeTypeError);
                if (!timeType.has_value()) {
                    addInitialConditionError(
                        "initial_conditions",
                        "InitialConditions.Time",
                        "initial-condition time expression is invalid: " + timeTypeError);
                } else if (!IsScalarTypeV2(*timeType)) {
                    addInitialConditionError(
                        "initial_conditions",
                        "InitialConditions.Time",
                        "initial-condition time expression must be scalar");
                }
            }

            std::set<Str> assignedStateIds;
            for (const auto &assignment : initialConditions.Assignments) {
                const auto entityId = assignment.StateDefinitionId.empty()
                    ? Str("initial_conditions")
                    : assignment.StateDefinitionId;

                if (assignment.StateDefinitionId.empty()) {
                    addInitialConditionError(entityId, "InitialConditions.Assignment", "initial-condition assignment target is empty");
                } else if (!assignedStateIds.insert(assignment.StateDefinitionId).second) {
                    addInitialConditionError(
                        entityId,
                        "InitialConditions.Assignment",
                        "duplicate initial-condition assignment for state '" + assignment.StateDefinitionId + "'");
                }

                const auto *stateDefinition = assignment.StateDefinitionId.empty()
                    ? nullptr
                    : FindDefinitionByIdV2(model, assignment.StateDefinitionId);
                if (stateDefinition == nullptr && !assignment.StateDefinitionId.empty()) {
                    addInitialConditionError(
                        entityId,
                        "InitialConditions.Assignment",
                        "initial-condition assignment references undefined definition '" + assignment.StateDefinitionId + "'");
                } else if (stateDefinition != nullptr && stateDefinition->Kind != EDefinitionKindV2::StateVariable) {
                    addInitialConditionError(
                        entityId,
                        "InitialConditions.Assignment",
                        "initial-condition assignment must target a state variable");
                }

                if (assignment.ValueExpression == nullptr) {
                    addInitialConditionError(entityId, "InitialConditions.Value", "initial-condition value expression is missing");
                    continue;
                }

                Str valueTypeError;
                const auto valueType = InferExpressionTypeV2(model, assignment.ValueExpression, &valueTypeError);
                if (!valueType.has_value()) {
                    addInitialConditionError(
                        entityId,
                        "InitialConditions.Value",
                        "initial-condition value expression is invalid: " + valueTypeError);
                    continue;
                }

                if (stateDefinition != nullptr &&
                    stateDefinition->DeclaredType.has_value() &&
                    stateDefinition->DeclaredType->Kind == ETypeExprKindV2::Function &&
                    stateDefinition->DeclaredType->Codomain != nullptr &&
                    !AreTypesEquivalentV2(*stateDefinition->DeclaredType->Codomain, *valueType)) {
                    addInitialConditionError(
                        entityId,
                        "InitialConditions.Value",
                        "initial-condition value type '" + RenderTypeExprV2(*valueType) +
                            "' does not match state codomain '" +
                            RenderTypeExprV2(*stateDefinition->DeclaredType->Codomain) + "'");
                }
            }
        }

        return result;
    }

} // namespace Slab::Core::Model::V2

#endif // STUDIOSLAB_MODEL_TYPES_V2_H
