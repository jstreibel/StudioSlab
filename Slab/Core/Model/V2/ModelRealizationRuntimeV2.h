#ifndef STUDIOSLAB_MODEL_REALIZATION_RUNTIME_V2_H
#define STUDIOSLAB_MODEL_REALIZATION_RUNTIME_V2_H

#include "ModelRealizationV2.h"

#include "Math/Numerics/ODE/Solver/BoundaryConditions.h"
#include "Math/Numerics/ODE/Solver/LinearStepSolver.h"
#include "Math/Numerics/ODE/Steppers/RungeKutta4.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"
#include "Math/Numerics/V2/Runtime/StepperSessionV2.h"
#include "Utils/Exception.h"

#include <cmath>
#include <cstdlib>
#include <limits>
#include <set>

namespace Slab::Core::Model::V2 {

    struct FODEExplicitFirstOrderRuntimeConfigV2 {
        DevFloat TimeStep = 0.01;
        TOptional<UIntBig> MaxSteps = std::nullopt;
        std::map<Str, DevFloat> ScalarBindingsByDefinitionId;
    };

    class FODEExplicitFirstOrderRuntimeSystemV2 {
        Str ModelId;
        Str ModelName;
        Str TimeCoordinateDefinitionId;
        Vector<Str> StateDefinitionIds;
        Vector<Str> StateDisplayLabels;
        std::map<Str, std::size_t> StateIndexByDefinitionId;
        Vector<FExpressionPtrV2> OrderedExplicitExpressions;
        std::map<Str, DevFloat> ScalarBindingsByDefinitionId;
        DevFloat InitialTime = 0.0;
        Vector<DevFloat> InitialStateValues;

    public:
        FODEExplicitFirstOrderRuntimeSystemV2() = default;

        [[nodiscard]] auto GetModelId() const -> const Str & {
            return ModelId;
        }

        [[nodiscard]] auto GetModelName() const -> const Str & {
            return ModelName;
        }

        auto SetModelId(Str modelId) -> void {
            ModelId = std::move(modelId);
        }

        auto SetModelName(Str modelName) -> void {
            ModelName = std::move(modelName);
        }

        auto SetTimeCoordinateDefinitionId(Str timeCoordinateDefinitionId) -> void {
            TimeCoordinateDefinitionId = std::move(timeCoordinateDefinitionId);
        }

        [[nodiscard]] auto GetTimeCoordinateDefinitionId() const -> const Str & {
            return TimeCoordinateDefinitionId;
        }

        auto SetScalarBindings(std::map<Str, DevFloat> scalarBindingsByDefinitionId) -> void {
            ScalarBindingsByDefinitionId = std::move(scalarBindingsByDefinitionId);
        }

        [[nodiscard]] auto GetScalarBindings() const -> const std::map<Str, DevFloat> & {
            return ScalarBindingsByDefinitionId;
        }

        auto SetInitialTime(const DevFloat initialTime) -> void {
            InitialTime = initialTime;
        }

        [[nodiscard]] auto GetInitialTime() const -> DevFloat {
            return InitialTime;
        }

        auto SetStateDefinitionIds(Vector<Str> stateDefinitionIds) -> void {
            StateDefinitionIds = std::move(stateDefinitionIds);
            StateIndexByDefinitionId.clear();
            for (std::size_t i = 0; i < StateDefinitionIds.size(); ++i) {
                StateIndexByDefinitionId[StateDefinitionIds[i]] = i;
            }
        }

        [[nodiscard]] auto GetStateDefinitionIds() const -> const Vector<Str> & {
            return StateDefinitionIds;
        }

        auto SetStateDisplayLabels(Vector<Str> stateDisplayLabels) -> void {
            StateDisplayLabels = std::move(stateDisplayLabels);
        }

        [[nodiscard]] auto GetStateDisplayLabels() const -> const Vector<Str> & {
            return StateDisplayLabels;
        }

        auto SetOrderedExplicitExpressions(Vector<FExpressionPtrV2> orderedExplicitExpressions) -> void {
            OrderedExplicitExpressions = std::move(orderedExplicitExpressions);
        }

        [[nodiscard]] auto GetOrderedExplicitExpressions() const -> const Vector<FExpressionPtrV2> & {
            return OrderedExplicitExpressions;
        }

        auto SetInitialStateValues(Vector<DevFloat> initialStateValues) -> void {
            InitialStateValues = std::move(initialStateValues);
        }

        [[nodiscard]] auto GetInitialStateValues() const -> const Vector<DevFloat> & {
            return InitialStateValues;
        }

        [[nodiscard]] auto FindStateIndex(const Str &stateDefinitionId) const -> TOptional<std::size_t> {
            const auto it = StateIndexByDefinitionId.find(stateDefinitionId);
            if (it == StateIndexByDefinitionId.end()) return std::nullopt;
            return it->second;
        }

        [[nodiscard]] auto ResolveRuntimeScalarSymbol(const Str &definitionId,
                                                      const Vector<DevFloat> *stateValues,
                                                      const DevFloat time,
                                                      const bool bAllowStateSymbols,
                                                      const bool bAllowTimeSymbol,
                                                      Str *pError = nullptr) const -> TOptional<DevFloat> {
            if (definitionId.empty()) {
                if (pError != nullptr) *pError = "runtime symbol binding is empty";
                return std::nullopt;
            }

            if (bAllowTimeSymbol && definitionId == TimeCoordinateDefinitionId) return time;

            if (const auto stateIndex = FindStateIndex(definitionId); stateIndex.has_value()) {
                if (!bAllowStateSymbols) {
                    if (pError != nullptr) {
                        *pError = "state symbol '" + definitionId + "' is not allowed in this runtime-evaluation context";
                    }
                    return std::nullopt;
                }
                if (stateValues == nullptr || *stateIndex >= stateValues->size()) {
                    if (pError != nullptr) {
                        *pError = "state symbol '" + definitionId + "' requires runtime state values";
                    }
                    return std::nullopt;
                }
                return (*stateValues)[*stateIndex];
            }

            const auto it = ScalarBindingsByDefinitionId.find(definitionId);
            if (it != ScalarBindingsByDefinitionId.end()) return it->second;

            if (pError != nullptr) *pError = "missing numeric binding for runtime symbol '" + definitionId + "'";
            return std::nullopt;
        }

        [[nodiscard]] auto EvaluateRhs(const Vector<DevFloat> &stateValues,
                                       const DevFloat time,
                                       Str *pError = nullptr) const -> TOptional<Vector<DevFloat>>;
    };

    DefinePointers(FODEExplicitFirstOrderRuntimeSystemV2)

    struct FODEExplicitFirstOrderRuntimeBuildResultV2 {
        FODEExplicitFirstOrderRuntimeSystemV2_ptr System = nullptr;
        Math::Numerics::V2::FSimulationRecipeV2_ptr Recipe = nullptr;
        Vector<FODERealizationDiagnosticV2> Diagnostics;

        [[nodiscard]] auto IsReady() const -> bool {
            const bool bHasBlockingDiagnostics = std::any_of(Diagnostics.begin(), Diagnostics.end(), [](const auto &diagnostic) {
                return diagnostic.Severity == EValidationSeverityV2::Error;
            });
            return !bHasBlockingDiagnostics && System != nullptr && Recipe != nullptr;
        }
    };

    namespace RuntimeDetail {

        inline auto CloneExpressionTreeV2(const FExpressionPtrV2 &expression) -> FExpressionPtrV2 {
            if (expression == nullptr) return nullptr;

            auto clone = New<FExpressionV2>();
            clone->Kind = expression->Kind;
            clone->SourceText = expression->SourceText;
            clone->Reference = expression->Reference;
            clone->LiteralText = expression->LiteralText;
            clone->UnaryOperator = expression->UnaryOperator;
            clone->BinaryOperator = expression->BinaryOperator;
            clone->DerivativeFlavor = expression->DerivativeFlavor;
            clone->DerivativeVariables = expression->DerivativeVariables;
            clone->bImplicitApplication = expression->bImplicitApplication;
            clone->Children.reserve(expression->Children.size());
            for (const auto &child : expression->Children) {
                clone->Children.push_back(CloneExpressionTreeV2(child));
            }
            return clone;
        }

        inline auto TryParseScalarLiteralV2(const Str &literalText) -> TOptional<DevFloat> {
            const auto trimmed = TrimAsciiCopyV2(literalText);
            if (trimmed.empty()) return std::nullopt;

            char *end = nullptr;
            const auto parsedValue = std::strtod(trimmed.c_str(), &end);
            if (end == trimmed.c_str() || end == nullptr || *end != '\0') return std::nullopt;
            return static_cast<DevFloat>(parsedValue);
        }

        inline auto EvaluateScalarExpressionV2(const FExpressionPtrV2 &expression,
                                               const FODEExplicitFirstOrderRuntimeSystemV2 &system,
                                               const Vector<DevFloat> *stateValues,
                                               const DevFloat time,
                                               const bool bAllowStateSymbols,
                                               const bool bAllowTimeSymbol,
                                               Str *pError = nullptr) -> TOptional<DevFloat> {
            const auto fail = [&](const Str &message) -> TOptional<DevFloat> {
                if (pError != nullptr) *pError = message;
                return std::nullopt;
            };

            if (expression == nullptr) return fail("runtime expression is null");

            switch (expression->Kind) {
                case EExpressionKindV2::Literal: {
                    const auto parsed = TryParseScalarLiteralV2(expression->LiteralText);
                    if (!parsed.has_value()) {
                        return fail("literal '" + expression->LiteralText + "' is not a runtime scalar literal");
                    }
                    return parsed;
                }
                case EExpressionKindV2::Symbol: {
                    if (!expression->Reference.IsBound()) {
                        return fail("runtime expression contains unresolved symbol '" + expression->Reference.DisplayText() + "'");
                    }
                    return system.ResolveRuntimeScalarSymbol(
                        expression->Reference.ReferenceId,
                        stateValues,
                        time,
                        bAllowStateSymbols,
                        bAllowTimeSymbol,
                        pError);
                }
                case EExpressionKindV2::Unary: {
                    if (expression->Children.size() != 1) return fail("unary runtime expression must have one child");
                    const auto value = EvaluateScalarExpressionV2(
                        expression->Children.front(),
                        system,
                        stateValues,
                        time,
                        bAllowStateSymbols,
                        bAllowTimeSymbol,
                        pError);
                    if (!value.has_value()) return std::nullopt;

                    switch (expression->UnaryOperator) {
                        case EUnaryOperatorV2::Plus: return *value;
                        case EUnaryOperatorV2::Minus: return -*value;
                    }
                    return fail("unsupported unary runtime operator");
                }
                case EExpressionKindV2::Binary: {
                    if (expression->Children.size() != 2) return fail("binary runtime expression must have two children");

                    const auto left = EvaluateScalarExpressionV2(
                        expression->Children[0],
                        system,
                        stateValues,
                        time,
                        bAllowStateSymbols,
                        bAllowTimeSymbol,
                        pError);
                    if (!left.has_value()) return std::nullopt;

                    const auto right = EvaluateScalarExpressionV2(
                        expression->Children[1],
                        system,
                        stateValues,
                        time,
                        bAllowStateSymbols,
                        bAllowTimeSymbol,
                        pError);
                    if (!right.has_value()) return std::nullopt;

                    switch (expression->BinaryOperator) {
                        case EBinaryOperatorV2::Add: return *left + *right;
                        case EBinaryOperatorV2::Subtract: return *left - *right;
                        case EBinaryOperatorV2::Multiply: return *left * *right;
                        case EBinaryOperatorV2::Divide:
                            if (std::abs(*right) <= std::numeric_limits<DevFloat>::epsilon()) {
                                return fail("runtime expression divides by zero");
                            }
                            return *left / *right;
                        case EBinaryOperatorV2::Power: return std::pow(*left, *right);
                    }
                    return fail("unsupported binary runtime operator");
                }
                case EExpressionKindV2::FunctionApplication:
                    return fail("runtime evaluator does not support function-application expressions yet");
                case EExpressionKindV2::OperatorApplication:
                    return fail("runtime evaluator does not support operator-application expressions yet");
                case EExpressionKindV2::Derivative:
                    return fail("runtime evaluator does not support derivative expressions in explicit RHS terms");
            }

            return fail("unsupported runtime expression kind");
        }

        inline auto CollectRuntimeExpressionDiagnosticsV2(const FExpressionPtrV2 &expression,
                                                          const Str &timeCoordinateDefinitionId,
                                                          const std::set<Str> &stateDefinitionIds,
                                                          const std::map<Str, DevFloat> &scalarBindingsByDefinitionId,
                                                          Vector<FODERealizationDiagnosticV2> &diagnostics) -> void {
            VisitExpressionDepthFirstV2(expression, [&](const FExpressionV2 &node) {
                if (node.Kind == EExpressionKindV2::FunctionApplication) {
                    RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                        diagnostics,
                        FODERealizationDiagnosticV2{
                            .Severity = EValidationSeverityV2::Error,
                            .Code = "unsupported_runtime_function_application",
                            .Source = std::nullopt,
                            .Message = "The first ODE runtime bridge does not support function-application expressions yet."
                        });
                    return;
                }

                if (node.Kind == EExpressionKindV2::OperatorApplication) {
                    RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                        diagnostics,
                        FODERealizationDiagnosticV2{
                            .Severity = EValidationSeverityV2::Error,
                            .Code = "unsupported_runtime_operator_application",
                            .Source = std::nullopt,
                            .Message = "The first ODE runtime bridge does not support operator-application expressions yet."
                        });
                    return;
                }

                if (node.Kind == EExpressionKindV2::Derivative) {
                    RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                        diagnostics,
                        FODERealizationDiagnosticV2{
                            .Severity = EValidationSeverityV2::Error,
                            .Code = "unsupported_runtime_derivative_expression",
                            .Source = std::nullopt,
                            .Message = "The first ODE runtime bridge expects explicit derivative-free RHS expressions."
                        });
                    return;
                }

                if (node.Kind != EExpressionKindV2::Symbol) return;
                if (!node.Reference.IsBound()) {
                    RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                        diagnostics,
                        FODERealizationDiagnosticV2{
                            .Severity = EValidationSeverityV2::Error,
                            .Code = "unresolved_runtime_symbol",
                            .Source = std::nullopt,
                            .Message = "The first ODE runtime bridge cannot evaluate unresolved symbol '" +
                                node.Reference.DisplayText() + "'."
                        });
                    return;
                }

                const auto &referenceId = node.Reference.ReferenceId;
                if (referenceId == timeCoordinateDefinitionId) return;
                if (stateDefinitionIds.contains(referenceId)) return;
                if (scalarBindingsByDefinitionId.contains(referenceId)) return;

                RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                    diagnostics,
                    FODERealizationDiagnosticV2{
                        .Severity = EValidationSeverityV2::Error,
                        .Code = "missing_numeric_binding",
                        .Source = MakeDefinitionObjectRefV2(referenceId),
                        .Message = "The first ODE runtime bridge requires one numeric binding for '" + referenceId + "'."
                    });
            });
        }

        class FScalarVectorEquationStateV2 final : public Math::Base::EquationState {
            Vector<DevFloat> Values;

            static auto Cast(const Math::Base::EquationState &state) -> const FScalarVectorEquationStateV2 & {
                return dynamic_cast<const FScalarVectorEquationStateV2 &>(state);
            }

        public:
            FScalarVectorEquationStateV2() = default;

            explicit FScalarVectorEquationStateV2(Vector<DevFloat> values)
            : Values(std::move(values)) {
            }

            [[nodiscard]] auto GetValues() const -> const Vector<DevFloat> & {
                return Values;
            }

            auto SetValues(Vector<DevFloat> values) -> void {
                Values = std::move(values);
            }

            auto StoreAddition(const Math::Base::EquationState &a,
                               const Math::Base::EquationState &b) -> Math::Base::EquationState & override {
                const auto &lhs = Cast(a);
                const auto &rhs = Cast(b);
                if (lhs.Values.size() != rhs.Values.size()) {
                    throw Exception("Scalar runtime state addition requires matching dimensions.");
                }

                Values.resize(lhs.Values.size(), 0.0);
                for (std::size_t i = 0; i < lhs.Values.size(); ++i) {
                    Values[i] = lhs.Values[i] + rhs.Values[i];
                }
                return *this;
            }

            auto StoreSubtraction(const Math::Base::EquationState &a,
                                  const Math::Base::EquationState &b) -> Math::Base::EquationState & override {
                const auto &lhs = Cast(a);
                const auto &rhs = Cast(b);
                if (lhs.Values.size() != rhs.Values.size()) {
                    throw Exception("Scalar runtime state subtraction requires matching dimensions.");
                }

                Values.resize(lhs.Values.size(), 0.0);
                for (std::size_t i = 0; i < lhs.Values.size(); ++i) {
                    Values[i] = lhs.Values[i] - rhs.Values[i];
                }
                return *this;
            }

            auto Add(const Math::Base::EquationState &a) -> Math::Base::EquationState & override {
                const auto &rhs = Cast(a);
                if (Values.size() != rhs.Values.size()) {
                    throw Exception("Scalar runtime state additive update requires matching dimensions.");
                }

                for (std::size_t i = 0; i < Values.size(); ++i) {
                    Values[i] += rhs.Values[i];
                }
                return *this;
            }

            auto Subtract(const Math::Base::EquationState &a) -> Math::Base::EquationState & override {
                const auto &rhs = Cast(a);
                if (Values.size() != rhs.Values.size()) {
                    throw Exception("Scalar runtime state subtractive update requires matching dimensions.");
                }

                for (std::size_t i = 0; i < Values.size(); ++i) {
                    Values[i] -= rhs.Values[i];
                }
                return *this;
            }

            auto StoreScalarMultiplication(const Math::Base::EquationState &a,
                                           const DevFloat factor) -> Math::Base::EquationState & override {
                const auto &rhs = Cast(a);
                Values.resize(rhs.Values.size(), 0.0);
                for (std::size_t i = 0; i < rhs.Values.size(); ++i) {
                    Values[i] = rhs.Values[i] * factor;
                }
                return *this;
            }

            auto Multiply(const DevFloat factor) -> Math::Base::EquationState & override {
                for (auto &value : Values) {
                    value *= factor;
                }
                return *this;
            }

            [[nodiscard]] auto Replicate(std::optional<Str> Name) const -> TPointer<Math::Base::EquationState> override {
                (void) Name;
                return New<FScalarVectorEquationStateV2>(Values);
            }

            [[nodiscard]] auto category() const -> Str override {
                return "model-v2|explicit-first-order-scalar-vector";
            }

            auto setData(const Math::Base::EquationState &other) -> void override {
                Values = Cast(other).Values;
            }
        };

        class FNoBoundaryConditionsV2 final : public Math::Base::BoundaryConditions {
        public:
            explicit FNoBoundaryConditionsV2(const Math::Base::EquationState_constptr &prototype)
            : BoundaryConditions(prototype) {
            }

            auto Apply(Math::Base::EquationState &toFunction, const DevFloat t) const -> void override {
                (void) toFunction;
                (void) t;
            }
        };

        class FExplicitFirstOrderRuntimeSolverV2 final : public Math::Base::LinearStepSolver {
            FODEExplicitFirstOrderRuntimeSystemV2_ptr System;

        public:
            explicit FExplicitFirstOrderRuntimeSolverV2(const FODEExplicitFirstOrderRuntimeSystemV2_ptr &system)
            : LinearStepSolver(New<FNoBoundaryConditionsV2>(
                New<FScalarVectorEquationStateV2>(system != nullptr ? system->GetInitialStateValues() : Vector<DevFloat>{})))
            , System(system) {
            }

            auto F(const Math::Base::EquationState &in,
                   Math::Base::EquationState &out,
                   const DevFloat t) -> Math::Base::EquationState & override {
                if (System == nullptr) throw Exception("Explicit first-order runtime solver requires a system.");

                const auto &inState = dynamic_cast<const FScalarVectorEquationStateV2 &>(in);
                auto &outState = dynamic_cast<FScalarVectorEquationStateV2 &>(out);

                Str error;
                const auto rhs = System->EvaluateRhs(inState.GetValues(), t, &error);
                if (!rhs.has_value()) {
                    throw Exception("Failed to evaluate explicit first-order runtime RHS: " + error);
                }

                outState.SetValues(*rhs);
                return out;
            }
        };

        class FExplicitFirstOrderSimulationRecipeV2 final : public Math::Numerics::V2::FSimulationRecipeV2 {
            FODEExplicitFirstOrderRuntimeSystemV2_ptr System;
            FODEExplicitFirstOrderRuntimeConfigV2 Config;

        public:
            FExplicitFirstOrderSimulationRecipeV2(FODEExplicitFirstOrderRuntimeSystemV2_ptr system,
                                                  FODEExplicitFirstOrderRuntimeConfigV2 config)
            : System(std::move(system))
            , Config(std::move(config)) {
            }

            auto BuildSession() -> TPointer<Math::Numerics::V2::FSimulationSessionV2> override {
                if (System == nullptr) {
                    throw Exception("Explicit first-order runtime recipe requires a system before building a session.");
                }

                auto solver = New<FExplicitFirstOrderRuntimeSolverV2>(System);
                auto stepper = New<Math::FRungeKutta4>(solver, Config.TimeStep);
                return New<Math::Numerics::V2::FStepperSessionV2>(
                    stepper,
                    Config.TimeStep,
                    0,
                    System->GetInitialTime());
            }

            auto BuildDefaultSubscriptions() -> Vector<Math::Numerics::V2::FSubscriptionV2> override {
                return {};
            }

            [[nodiscard]] auto GetRunLimits() const -> Math::Numerics::V2::FRunLimitsV2 override {
                Math::Numerics::V2::FRunLimitsV2 limits;
                if (Config.MaxSteps.has_value()) {
                    limits.Mode = Math::Numerics::V2::ERunModeV2::FiniteSteps;
                    limits.MaxSteps = Config.MaxSteps;
                } else {
                    limits.Mode = Math::Numerics::V2::ERunModeV2::OpenEnded;
                    limits.MaxSteps = std::nullopt;
                }
                return limits;
            }
        };

    } // namespace RuntimeDetail

    inline auto FODEExplicitFirstOrderRuntimeSystemV2::EvaluateRhs(const Vector<DevFloat> &stateValues,
                                                                   const DevFloat time,
                                                                   Str *pError) const -> TOptional<Vector<DevFloat>> {
        const auto fail = [&](const Str &message) -> TOptional<Vector<DevFloat>> {
            if (pError != nullptr) *pError = message;
            return std::nullopt;
        };

        if (stateValues.size() != OrderedExplicitExpressions.size()) {
            return fail("runtime state dimension does not match the ordered explicit relation set");
        }

        Vector<DevFloat> rhs(OrderedExplicitExpressions.size(), 0.0);
        for (std::size_t i = 0; i < OrderedExplicitExpressions.size(); ++i) {
            Str error;
            const auto value = RuntimeDetail::EvaluateScalarExpressionV2(
                OrderedExplicitExpressions[i],
                *this,
                &stateValues,
                time,
                true,
                true,
                &error);
            if (!value.has_value()) {
                return fail("state '" + StateDefinitionIds[i] + "' explicit RHS failed: " + error);
            }
            rhs[i] = *value;
        }
        return rhs;
    }

    inline auto TryExtractODEExplicitFirstOrderRuntimeStateValuesV2(const Math::Base::EquationState &state)
        -> TOptional<Vector<DevFloat>> {
        const auto *runtimeState = dynamic_cast<const RuntimeDetail::FScalarVectorEquationStateV2 *>(&state);
        if (runtimeState == nullptr) return std::nullopt;
        return runtimeState->GetValues();
    }

    inline auto CollectODEExplicitFirstOrderRequiredScalarBindingsV2(const FModelV2 &model,
                                                                     const FODERealizationDescriptorV2 &descriptor)
        -> Vector<FODERealizationSymbolV2> {
        Vector<FODERealizationSymbolV2> requiredBindings;
        if (!descriptor.IsReady()) return requiredBindings;
        if (descriptor.Strategy != EODERealizationStrategyV2::ExplicitFirstOrderSystem) return requiredBindings;
        if (!descriptor.TimeCoordinate.has_value()) return requiredBindings;

        const auto timeCoordinateDefinitionId = descriptor.TimeCoordinate->DefinitionId;
        const auto stateDefinitionIds = [&]() {
            std::set<Str> ids;
            for (const auto &state : descriptor.StateVariables) {
                ids.insert(state.DefinitionId);
            }
            return ids;
        }();

        std::set<Str> appendedDefinitionIds;
        const auto appendBinding = [&](const Str &definitionId) -> void {
            if (definitionId.empty()) return;
            if (definitionId == timeCoordinateDefinitionId) return;
            if (stateDefinitionIds.contains(definitionId)) return;
            if (!appendedDefinitionIds.insert(definitionId).second) return;

            FODERealizationSymbolV2 symbol;
            symbol.DefinitionId = definitionId;

            if (const auto parameterIt = std::find_if(
                    descriptor.Parameters.begin(),
                    descriptor.Parameters.end(),
                    [&](const auto &parameter) {
                        return parameter.DefinitionId == definitionId;
                    });
                parameterIt != descriptor.Parameters.end()) {
                symbol = *parameterIt;
            } else if (const auto *definition = FindDefinitionByIdV2(model, definitionId); definition != nullptr) {
                symbol.DisplayLabel = !definition->DisplayName.empty()
                    ? definition->DisplayName
                    : (!definition->PreferredNotation.empty() ? definition->PreferredNotation : definitionId);
                symbol.CanonicalNotation = MakeCanonicalDefinitionNotationV2(*definition, &model);
            } else {
                symbol.DisplayLabel = definitionId;
                symbol.CanonicalNotation = definitionId;
            }

            if (symbol.DisplayLabel.empty()) symbol.DisplayLabel = definitionId;
            if (symbol.CanonicalNotation.empty()) symbol.CanonicalNotation = definitionId;
            requiredBindings.push_back(std::move(symbol));
        };

        const auto collectExpressionBindings = [&](const FExpressionPtrV2 &expression,
                                                  std::set<Str> &bindingIds) -> void {
            VisitExpressionDepthFirstV2(expression, [&](const FExpressionV2 &node) {
                if (node.Kind != EExpressionKindV2::Symbol) return;
                if (!node.Reference.IsBound()) return;
                const auto &referenceId = node.Reference.ReferenceId;
                if (referenceId.empty()) return;
                if (referenceId == timeCoordinateDefinitionId) return;
                if (stateDefinitionIds.contains(referenceId)) return;
                bindingIds.insert(referenceId);
            });
        };

        std::set<Str> relationBindingIds;
        for (const auto &selectedRelation : descriptor.SelectedRelations) {
            const auto *relation = FindRelationByIdV2(model, selectedRelation.RelationId);
            if (relation == nullptr) continue;

            const auto explicitExpression = selectedRelation.DerivativeSide == EODEExplicitRelationSideV2::Left
                ? relation->Right
                : relation->Left;
            collectExpressionBindings(explicitExpression, relationBindingIds);
        }

        std::set<Str> initialBindingIds;
        if (model.InitialConditions.has_value()) {
            collectExpressionBindings(model.InitialConditions->TimeExpression, initialBindingIds);
            for (const auto &state : descriptor.StateVariables) {
                const auto assignmentIt = std::find_if(
                    model.InitialConditions->Assignments.begin(),
                    model.InitialConditions->Assignments.end(),
                    [&](const auto &assignment) {
                        return assignment.StateDefinitionId == state.DefinitionId;
                    });
                if (assignmentIt == model.InitialConditions->Assignments.end()) continue;
                collectExpressionBindings(assignmentIt->ValueExpression, initialBindingIds);
            }
        }

        const auto appendBindingsInModelDefinitionOrder = [&](const std::set<Str> &bindingIds) -> void {
            for (const auto &definition : model.Definitions) {
                if (!bindingIds.contains(definition.DefinitionId)) continue;
                appendBinding(definition.DefinitionId);
            }

            for (const auto &definitionId : bindingIds) {
                appendBinding(definitionId);
            }
        };

        appendBindingsInModelDefinitionOrder(relationBindingIds);
        appendBindingsInModelDefinitionOrder(initialBindingIds);

        return requiredBindings;
    }

    inline auto BuildODEExplicitFirstOrderRuntimeV2(const FModelV2 &model,
                                                    const FODERealizationDescriptorV2 &descriptor,
                                                    const FODEExplicitFirstOrderRuntimeConfigV2 &config)
        -> FODEExplicitFirstOrderRuntimeBuildResultV2 {
        FODEExplicitFirstOrderRuntimeBuildResultV2 result;

        if (!descriptor.IsReady()) {
            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                result.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Error,
                    .Code = "descriptor_not_ready",
                    .Source = std::nullopt,
                    .Message = "Explicit first-order runtime bridge requires an ODE descriptor that is already ready."
                });
            return result;
        }

        if (descriptor.Strategy != EODERealizationStrategyV2::ExplicitFirstOrderSystem) {
            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                result.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Error,
                    .Code = "unsupported_runtime_strategy",
                    .Source = std::nullopt,
                    .Message = "The first runtime bridge only supports explicit first-order ODE systems."
                });
            return result;
        }

        if (!descriptor.TimeCoordinate.has_value()) {
            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                result.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Error,
                    .Code = "missing_runtime_time_coordinate",
                    .Source = std::nullopt,
                    .Message = "Explicit first-order runtime bridge requires one time coordinate."
                });
            return result;
        }

        if (!(config.TimeStep > 0.0)) {
            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                result.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Error,
                    .Code = "invalid_runtime_time_step",
                    .Source = std::nullopt,
                    .Message = "Explicit first-order runtime bridge requires a positive time step."
                });
            return result;
        }

        auto system = New<FODEExplicitFirstOrderRuntimeSystemV2>();
        system->SetModelId(descriptor.ModelId);
        system->SetModelName(descriptor.ModelName);
        system->SetTimeCoordinateDefinitionId(descriptor.TimeCoordinate->DefinitionId);
        system->SetScalarBindings(config.ScalarBindingsByDefinitionId);

        Vector<Str> stateDefinitionIds;
        Vector<Str> stateDisplayLabels;
        stateDefinitionIds.reserve(descriptor.StateVariables.size());
        stateDisplayLabels.reserve(descriptor.StateVariables.size());
        for (const auto &state : descriptor.StateVariables) {
            stateDefinitionIds.push_back(state.DefinitionId);
            stateDisplayLabels.push_back(state.DisplayLabel);
        }
        system->SetStateDefinitionIds(std::move(stateDefinitionIds));
        system->SetStateDisplayLabels(std::move(stateDisplayLabels));

        const std::set<Str> stateDefinitionIdSet(
            system->GetStateDefinitionIds().begin(),
            system->GetStateDefinitionIds().end());

        Vector<FExpressionPtrV2> orderedExplicitExpressions;
        orderedExplicitExpressions.reserve(system->GetStateDefinitionIds().size());
        for (const auto &stateDefinitionId : system->GetStateDefinitionIds()) {
            const auto relationIt = std::find_if(
                descriptor.SelectedRelations.begin(),
                descriptor.SelectedRelations.end(),
                [&](const auto &relation) {
                    return relation.StateDefinitionId == stateDefinitionId;
                });

            if (relationIt == descriptor.SelectedRelations.end()) {
                RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                    result.Diagnostics,
                    FODERealizationDiagnosticV2{
                        .Severity = EValidationSeverityV2::Error,
                        .Code = "missing_runtime_selected_relation",
                        .Source = MakeDefinitionObjectRefV2(stateDefinitionId),
                        .Message = "Explicit first-order runtime bridge could not find the selected relation for state '" +
                            stateDefinitionId + "'."
                    });
                continue;
            }

            const auto *relation = FindRelationByIdV2(model, relationIt->RelationId);
            if (relation == nullptr) {
                RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                    result.Diagnostics,
                    FODERealizationDiagnosticV2{
                        .Severity = EValidationSeverityV2::Error,
                        .Code = "missing_runtime_relation",
                        .Source = MakeRelationObjectRefV2(relationIt->RelationId),
                        .Message = "Explicit first-order runtime bridge could not find canonical relation '" +
                            relationIt->RelationId + "' in the model."
                    });
                continue;
            }

            const auto explicitExpression = relationIt->DerivativeSide == EODEExplicitRelationSideV2::Left
                ? relation->Right
                : relation->Left;
            orderedExplicitExpressions.push_back(RuntimeDetail::CloneExpressionTreeV2(explicitExpression));
        }

        if (orderedExplicitExpressions.size() != system->GetStateDefinitionIds().size()) return result;

        for (const auto &expression : orderedExplicitExpressions) {
            RuntimeDetail::CollectRuntimeExpressionDiagnosticsV2(
                expression,
                system->GetTimeCoordinateDefinitionId(),
                stateDefinitionIdSet,
                config.ScalarBindingsByDefinitionId,
                result.Diagnostics);
        }

        if (!model.InitialConditions.has_value()) {
            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                result.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Error,
                    .Code = "missing_runtime_initial_conditions",
                    .Source = std::nullopt,
                    .Message = "Explicit first-order runtime bridge requires one model-level initial-condition set."
                });
            return result;
        }

        auto initialTimeExpression = RuntimeDetail::CloneExpressionTreeV2(model.InitialConditions->TimeExpression);
        RuntimeDetail::CollectRuntimeExpressionDiagnosticsV2(
            initialTimeExpression,
            system->GetTimeCoordinateDefinitionId(),
            stateDefinitionIdSet,
            config.ScalarBindingsByDefinitionId,
            result.Diagnostics);

        Vector<FExpressionPtrV2> initialStateExpressions;
        initialStateExpressions.reserve(system->GetStateDefinitionIds().size());
        for (const auto &stateDefinitionId : system->GetStateDefinitionIds()) {
            const auto assignmentIt = std::find_if(
                model.InitialConditions->Assignments.begin(),
                model.InitialConditions->Assignments.end(),
                [&](const auto &assignment) {
                    return assignment.StateDefinitionId == stateDefinitionId;
                });

            if (assignmentIt == model.InitialConditions->Assignments.end()) {
                RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                    result.Diagnostics,
                    FODERealizationDiagnosticV2{
                        .Severity = EValidationSeverityV2::Error,
                        .Code = "missing_runtime_initial_assignment",
                        .Source = MakeDefinitionObjectRefV2(stateDefinitionId),
                        .Message = "Explicit first-order runtime bridge could not find the model-level initial assignment for state '" +
                            stateDefinitionId + "'."
                    });
                continue;
            }

            auto expression = RuntimeDetail::CloneExpressionTreeV2(assignmentIt->ValueExpression);
            RuntimeDetail::CollectRuntimeExpressionDiagnosticsV2(
                expression,
                system->GetTimeCoordinateDefinitionId(),
                stateDefinitionIdSet,
                config.ScalarBindingsByDefinitionId,
                result.Diagnostics);
            initialStateExpressions.push_back(std::move(expression));
        }

        if (initialStateExpressions.size() != system->GetStateDefinitionIds().size()) return result;

        const bool bHasBlockingDiagnostics = std::any_of(result.Diagnostics.begin(), result.Diagnostics.end(), [](const auto &diagnostic) {
            return diagnostic.Severity == EValidationSeverityV2::Error;
        });
        if (bHasBlockingDiagnostics) return result;

        Str initialTimeError;
        const auto initialTime = RuntimeDetail::EvaluateScalarExpressionV2(
            initialTimeExpression,
            *system,
            nullptr,
            0.0,
            false,
            false,
            &initialTimeError);
        if (!initialTime.has_value()) {
            RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                result.Diagnostics,
                FODERealizationDiagnosticV2{
                    .Severity = EValidationSeverityV2::Error,
                    .Code = "invalid_runtime_initial_time",
                    .Source = std::nullopt,
                    .Message = "Explicit first-order runtime bridge could not evaluate the initial time expression: " +
                        initialTimeError
                });
            return result;
        }
        system->SetInitialTime(*initialTime);

        Vector<DevFloat> initialStateValues(initialStateExpressions.size(), 0.0);
        for (std::size_t i = 0; i < initialStateExpressions.size(); ++i) {
            Str initialValueError;
            const auto value = RuntimeDetail::EvaluateScalarExpressionV2(
                initialStateExpressions[i],
                *system,
                nullptr,
                *initialTime,
                false,
                true,
                &initialValueError);
            if (!value.has_value()) {
                RealizationDetail::AppendRealizationDiagnosticUniqueV2(
                    result.Diagnostics,
                    FODERealizationDiagnosticV2{
                        .Severity = EValidationSeverityV2::Error,
                        .Code = "invalid_runtime_initial_value",
                        .Source = MakeDefinitionObjectRefV2(system->GetStateDefinitionIds()[i]),
                        .Message = "Explicit first-order runtime bridge could not evaluate the initial value for state '" +
                            system->GetStateDefinitionIds()[i] + "': " + initialValueError
                    });
                return result;
            }
            initialStateValues[i] = *value;
        }

        system->SetInitialStateValues(std::move(initialStateValues));
        system->SetOrderedExplicitExpressions(std::move(orderedExplicitExpressions));

        result.System = system;
        result.Recipe = New<RuntimeDetail::FExplicitFirstOrderSimulationRecipeV2>(system, config);
        return result;
    }

} // namespace Slab::Core::Model::V2

#endif // STUDIOSLAB_MODEL_REALIZATION_RUNTIME_V2_H
