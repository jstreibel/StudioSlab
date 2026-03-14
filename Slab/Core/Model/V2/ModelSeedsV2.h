#ifndef STUDIOSLAB_MODEL_SEEDS_V2_H
#define STUDIOSLAB_MODEL_SEEDS_V2_H

#include "ModelNotationV2.h"

namespace Slab::Core::Model::V2 {

    namespace Detail {

        inline auto RequireParsedDefinitionV2(const Str &source, const FNotationContextV2 *context = nullptr)
            -> FParsedDefinitionNotationV2 {
            const auto parsed = ParseDefinitionNotationV2(source, context);
            if (!parsed.IsOk()) {
                throw Exception(
                    "Failed to parse model definition notation '" + source +
                    "' at " + ToStr(parsed.Error.Position) + ": " + parsed.Error.Message);
            }
            return *parsed.Value;
        }

        inline auto RequireParsedRelationV2(const Str &relationId,
                                            const ERelationKindV2 kind,
                                            const Str &source,
                                            const FNotationContextV2 *context = nullptr) -> FRelationV2 {
            const auto parsed = ParseRelationNotationV2(relationId, kind, source, context);
            if (!parsed.IsOk()) {
                throw Exception(
                    "Failed to parse model relation notation '" + source +
                    "' at " + ToStr(parsed.Error.Position) + ": " + parsed.Error.Message);
            }
            return *parsed.Value;
        }

        inline auto RequireParsedExpressionV2(const Str &source, const FNotationContextV2 *context = nullptr)
            -> FExpressionPtrV2 {
            const auto parsed = ParseExpressionNotationV2(source, context);
            if (!parsed.IsOk()) {
                throw Exception(
                    "Failed to parse model expression notation '" + source +
                    "' at " + ToStr(parsed.Error.Position) + ": " + parsed.Error.Message);
            }
            return *parsed.Value;
        }

    } // namespace Detail

    inline auto BuildHarmonicOscillatorModelV2() -> FModelV2 {
        FModelV2 model;
        model.ModelId = "model.harmonic_oscillator";
        model.Name = "Harmonic Oscillator";
        model.Description =
            "Finite-dimensional ODE model with one scalar displacement, one scalar momentum, "
            "symbolic parameters, and equivalent second-order and first-order relations.";
        model.BaseVocabulary.ActivePresetId = "classical_mechanics";
        model.Tags = {"model", "ode", "classical-mechanics"};

        {
            auto parsed = Detail::RequireParsedDefinitionV2("t \\in \\mathbb{R}");
            auto definition = MakeDefinitionFromParsedNotationV2(
                "coord.t",
                EDefinitionKindV2::Coordinate,
                parsed,
                "Time",
                "Time coordinate for the oscillator.");
            definition.CoordinateRole = ECoordinateRoleV2::Time;
            model.Definitions.push_back(std::move(definition));
        }

        {
            auto parsed = Detail::RequireParsedDefinitionV2("m \\in \\mathbb{R}");
            auto definition = MakeDefinitionFromParsedNotationV2(
                "param.m",
                EDefinitionKindV2::ScalarParameter,
                parsed,
                "Mass",
                "Symbolic mass parameter.");
            definition.Tags = {"parameter"};
            model.Definitions.push_back(std::move(definition));
        }

        {
            auto parsed = Detail::RequireParsedDefinitionV2("k \\in \\mathbb{R}");
            auto definition = MakeDefinitionFromParsedNotationV2(
                "param.k",
                EDefinitionKindV2::ScalarParameter,
                parsed,
                "Spring Constant",
                "Symbolic stiffness parameter.");
            definition.Tags = {"parameter"};
            model.Definitions.push_back(std::move(definition));
        }

        {
            auto parsed = Detail::RequireParsedDefinitionV2("\\omega \\in \\mathbb{R}");
            auto definition = MakeDefinitionFromParsedNotationV2(
                "param.omega",
                EDefinitionKindV2::ScalarParameter,
                parsed,
                "Angular Frequency",
                "Symbolic angular frequency.");
            definition.Tags = {"parameter"};
            model.Definitions.push_back(std::move(definition));
        }

        {
            auto parsed = Detail::RequireParsedDefinitionV2("x_0 \\in \\mathbb{R}");
            auto definition = MakeDefinitionFromParsedNotationV2(
                "param.x0",
                EDefinitionKindV2::ScalarParameter,
                parsed,
                "Initial Displacement",
                "Symbolic initial displacement parameter.");
            definition.Tags = {"parameter", "initial-condition"};
            model.Definitions.push_back(std::move(definition));
        }

        {
            auto parsed = Detail::RequireParsedDefinitionV2("p_0 \\in \\mathbb{R}");
            auto definition = MakeDefinitionFromParsedNotationV2(
                "param.p0",
                EDefinitionKindV2::ScalarParameter,
                parsed,
                "Initial Momentum",
                "Symbolic initial momentum parameter.");
            definition.Tags = {"parameter", "initial-condition"};
            model.Definitions.push_back(std::move(definition));
        }

        {
            const auto context = FNotationContextV2::FromModel(model);
            auto parsed = Detail::RequireParsedDefinitionV2("x : \\mathbb{R} \\to \\mathbb{R}", &context);
            auto definition = MakeDefinitionFromParsedNotationV2(
                "state.x",
                EDefinitionKindV2::StateVariable,
                parsed,
                "Displacement",
                "Scalar configuration coordinate x(t).",
                {"coord.t"});
            definition.Tags = {"state", "configuration"};
            model.Definitions.push_back(std::move(definition));
        }

        {
            const auto context = FNotationContextV2::FromModel(model);
            auto parsed = Detail::RequireParsedDefinitionV2("p : \\mathbb{R} \\to \\mathbb{R}", &context);
            auto definition = MakeDefinitionFromParsedNotationV2(
                "state.p",
                EDefinitionKindV2::StateVariable,
                parsed,
                "Momentum",
                "Scalar momentum p(t).",
                {"coord.t"});
            definition.Tags = {"state", "momentum"};
            model.Definitions.push_back(std::move(definition));
        }

        {
            const auto context = FNotationContextV2::FromModel(model);
            auto parsed = Detail::RequireParsedDefinitionV2("E : \\mathbb{R} \\to \\mathbb{R}", &context);
            auto definition = MakeDefinitionFromParsedNotationV2(
                "obs.energy",
                EDefinitionKindV2::ObservableSymbol,
                parsed,
                "Energy",
                "Mechanical energy observable along the trajectory.",
                {"coord.t"},
                {"state.x", "state.p", "param.m", "param.k"});
            definition.Tags = {"observable", "energy"};
            model.Definitions.push_back(std::move(definition));
        }

        {
            const auto context = FNotationContextV2::FromModel(model);
            auto relation = Detail::RequireParsedRelationV2(
                "relation.oscillator.second_order",
                ERelationKindV2::DifferentialEquation,
                "\\ddot x + \\omega^2 x = 0",
                &context);
            relation.Name = "Second-order Equation";
            relation.Description = "Platonic second-order oscillator equation.";
            relation.Tags = {"equation", "ode"};
            model.Relations.push_back(std::move(relation));
        }

        {
            const auto context = FNotationContextV2::FromModel(model);
            auto relation = Detail::RequireParsedRelationV2(
                "relation.oscillator.first_order_x",
                ERelationKindV2::DifferentialEquation,
                "\\dot x = p / m",
                &context);
            relation.Name = "First-order Position Equation";
            relation.Description = "Canonical first-order form for x.";
            relation.Tags = {"equation", "ode", "first-order"};
            model.Relations.push_back(std::move(relation));
        }

        {
            const auto context = FNotationContextV2::FromModel(model);
            auto relation = Detail::RequireParsedRelationV2(
                "relation.oscillator.first_order_p",
                ERelationKindV2::DifferentialEquation,
                "\\dot p = -k x",
                &context);
            relation.Name = "First-order Momentum Equation";
            relation.Description = "Canonical first-order form for p.";
            relation.Tags = {"equation", "ode", "first-order"};
            model.Relations.push_back(std::move(relation));
        }

        {
            const auto context = FNotationContextV2::FromModel(model);
            auto relation = Detail::RequireParsedRelationV2(
                "relation.oscillator.frequency_constraint",
                ERelationKindV2::Constraint,
                "\\omega^2 = k / m",
                &context);
            relation.Name = "Frequency Constraint";
            relation.Description = "Symbolic relation between k, m, and omega.";
            relation.Tags = {"constraint"};
            model.Relations.push_back(std::move(relation));
        }

        {
            const auto context = FNotationContextV2::FromModel(model);
            auto relation = Detail::RequireParsedRelationV2(
                "relation.oscillator.energy",
                ERelationKindV2::Identity,
                "E = p^2 / (2 m) + k x^2 / 2",
                &context);
            relation.Name = "Energy Observable";
            relation.Description = "Symbolic energy observable over the trajectory.";
            relation.Tags = {"observable", "identity"};
            model.Relations.push_back(std::move(relation));
        }

        {
            const auto context = FNotationContextV2::FromModel(model);
            FModelInitialConditionSetV2 initialConditions;
            initialConditions.TimeExpression = Detail::RequireParsedExpressionV2("0", &context);
            initialConditions.Description = "Canonical initial state for the oscillator ODE slice.";
            initialConditions.Assignments.push_back(FModelInitialConditionAssignmentV2{
                .StateDefinitionId = "state.x",
                .ValueExpression = Detail::RequireParsedExpressionV2("x_0", &context),
                .Description = "Initial displacement."
            });
            initialConditions.Assignments.push_back(FModelInitialConditionAssignmentV2{
                .StateDefinitionId = "state.p",
                .ValueExpression = Detail::RequireParsedExpressionV2("p_0", &context),
                .Description = "Initial momentum."
            });
            model.InitialConditions = std::move(initialConditions);
        }

        return model;
    }

    inline auto BuildDampedHarmonicOscillatorModelV2() -> FModelV2 {
        auto model = BuildHarmonicOscillatorModelV2();
        model.ModelId = "model.damped_harmonic_oscillator";
        model.Name = "Damped Harmonic Oscillator";
        model.Description =
            "Finite-dimensional ODE model with explicit linear damping, canonical first-order state equations, "
            "symbolic parameters, and one explicit model-level initial state.";
        model.Tags = {"model", "ode", "classical-mechanics", "damped"};

        {
            auto parsed = Detail::RequireParsedDefinitionV2("\\gamma \\in \\mathbb{R}");
            auto definition = MakeDefinitionFromParsedNotationV2(
                "param.gamma",
                EDefinitionKindV2::ScalarParameter,
                parsed,
                "Damping Coefficient",
                "Linear damping coefficient for the oscillator.");
            definition.Tags = {"parameter", "damping"};
            model.Definitions.push_back(std::move(definition));
        }

        {
            const auto context = FNotationContextV2::FromModel(model);
            auto dampedSecondOrder = Detail::RequireParsedRelationV2(
                "relation.oscillator.second_order",
                ERelationKindV2::DifferentialEquation,
                "\\ddot x + \\gamma \\dot x + \\omega^2 x = 0",
                &context);
            dampedSecondOrder.Name = "Damped Second-order Equation";
            dampedSecondOrder.Description = "Platonic damped second-order oscillator equation.";
            dampedSecondOrder.Tags = {"equation", "ode", "damped"};

            if (auto *relation = FindRelationByIdV2(model, "relation.oscillator.second_order"); relation != nullptr) {
                *relation = std::move(dampedSecondOrder);
            }
        }

        {
            const auto context = FNotationContextV2::FromModel(model);
            auto dampedMomentum = Detail::RequireParsedRelationV2(
                "relation.oscillator.first_order_p",
                ERelationKindV2::DifferentialEquation,
                "\\dot p = -\\gamma p - k x",
                &context);
            dampedMomentum.Name = "Damped First-order Momentum Equation";
            dampedMomentum.Description = "Canonical first-order form for p with linear damping.";
            dampedMomentum.Tags = {"equation", "ode", "first-order", "damped"};

            if (auto *relation = FindRelationByIdV2(model, "relation.oscillator.first_order_p"); relation != nullptr) {
                *relation = std::move(dampedMomentum);
            }
        }

        return model;
    }

    inline auto BuildKleinGordonModelV2() -> FModelV2 {
        FModelV2 model;
        model.ModelId = "model.klein_gordon";
        model.Name = "Klein-Gordon Field";
        model.Description =
            "Field-theoretic model with an abstract scalar field over symbolic spacetime dimension "
            "and an ambient d'Alembertian operator from the active vocabulary.";
        model.BaseVocabulary.ActivePresetId = "relativistic_field_theory";
        model.Tags = {"model", "pde", "field-theory", "spacetime"};
        model.Metadata["ambient_space"] = "Minkowski-like";

        {
            auto parsed = Detail::RequireParsedDefinitionV2("d \\in \\mathbb{R}");
            auto definition = MakeDefinitionFromParsedNotationV2(
                "param.d",
                EDefinitionKindV2::ScalarParameter,
                parsed,
                "Spatial Dimension Symbol",
                "Open symbolic spatial dimension used in the field domain.");
            definition.Tags = {"parameter", "dimension"};
            model.Definitions.push_back(std::move(definition));
        }

        {
            auto parsed = Detail::RequireParsedDefinitionV2("m \\in \\mathbb{R}");
            auto definition = MakeDefinitionFromParsedNotationV2(
                "param.m",
                EDefinitionKindV2::ScalarParameter,
                parsed,
                "Mass",
                "Symbolic Klein-Gordon mass parameter.");
            definition.Tags = {"parameter", "mass"};
            model.Definitions.push_back(std::move(definition));
        }

        {
            const auto context = FNotationContextV2::FromModel(model);
            auto parsed = Detail::RequireParsedDefinitionV2("\\phi : \\mathbb{R}^{d+1} \\to \\mathbb{R}", &context);
            auto definition = MakeDefinitionFromParsedNotationV2(
                "field.phi",
                EDefinitionKindV2::Field,
                parsed,
                "Scalar Field",
                "Abstract scalar field over symbolic spacetime.");
            definition.Tags = {"field", "scalar"};
            definition.Metadata["ambient_space"] = "Minkowski-like";
            model.Definitions.push_back(std::move(definition));
        }

        {
            const auto context = FNotationContextV2::FromModel(model);
            auto relation = Detail::RequireParsedRelationV2(
                "relation.klein_gordon.equation",
                ERelationKindV2::OperatorEquation,
                "\\Box \\phi + m^2 \\phi = 0",
                &context);
            relation.Name = "Klein-Gordon Equation";
            relation.Description = "Abstract Klein-Gordon field equation with symbolic mass.";
            relation.Tags = {"equation", "operator", "pde"};
            model.Relations.push_back(std::move(relation));
        }

        return model;
    }

    inline auto BuildDemoModelsV2() -> Vector<FModelV2> {
        return {
            BuildHarmonicOscillatorModelV2(),
            BuildDampedHarmonicOscillatorModelV2(),
            BuildKleinGordonModelV2()
        };
    }

} // namespace Slab::Core::Model::V2

#endif // STUDIOSLAB_MODEL_SEEDS_V2_H
