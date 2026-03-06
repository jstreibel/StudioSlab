#ifndef STUDIOSLAB_SEMANTIC_TYPES_V1_H
#define STUDIOSLAB_SEMANTIC_TYPES_V1_H

#include "ReflectionCodecsV2.h"

#include <algorithm>
#include <cstdint>
#include <map>
#include <optional>
#include <utility>

namespace Slab::Core::Reflection::V2 {

    inline constexpr auto CSemanticCatalogVersionV1 = "0.1";

    using FSpaceId = Str;
    using FSemanticOperatorId = Str;

    // Placeholder carriers for spaces that currently do not have a stable Reflection V2 TypeId.
    inline constexpr auto CTypeIdVectorRealOpaqueV1 = "slab.semantic.carrier.vector_real.opaque";
    inline constexpr auto CTypeIdField2DRealOpaqueV1 = "slab.semantic.carrier.field2d_real.opaque";
    inline constexpr auto CTypeIdSpectrum1DComplexOpaqueV1 = "slab.semantic.carrier.spectrum1d_complex.opaque";
    inline constexpr auto CTypeIdVisualizationHandleOpaqueV1 = "slab.semantic.carrier.visualization_handle.opaque";
    inline constexpr auto CTypeIdUnitOpaqueV1 = "slab.semantic.carrier.unit";

    inline constexpr auto CSpaceIdScalarRealV1 = "slab.semantic.space.scalar_real";
    inline constexpr auto CSpaceIdScalarIntV1 = "slab.semantic.space.scalar_int";
    inline constexpr auto CSpaceIdBoolV1 = "slab.semantic.space.bool";
    inline constexpr auto CSpaceIdVectorRealV1 = "slab.semantic.space.vector_real";
    inline constexpr auto CSpaceIdField2DRealV1 = "slab.semantic.space.field2d_real";
    inline constexpr auto CSpaceIdSpectrum1DComplexV1 = "slab.semantic.space.spectrum1d_complex";
    inline constexpr auto CSpaceIdFunctionRtoRV1 = "slab.semantic.space.function_r_to_r";
    inline constexpr auto CSpaceIdVisualizationHandleV1 = "slab.semantic.space.visualization_handle";
    inline constexpr auto CSpaceIdUnitV1 = "slab.semantic.space.unit";

    inline constexpr auto COperatorIdIdentityV1 = "slab.semantic.operator.identity";
    inline constexpr auto COperatorIdFourierTransformV1 = "slab.semantic.operator.fourier_transform";
    inline constexpr auto COperatorIdInverseFourierTransformV1 = "slab.semantic.operator.inverse_fourier_transform";
    inline constexpr auto COperatorIdLaplacianV1 = "slab.semantic.operator.laplacian";
    inline constexpr auto COperatorIdGradientV1 = "slab.semantic.operator.gradient";
    inline constexpr auto COperatorIdAverageV1 = "slab.semantic.operator.average";
    inline constexpr auto COperatorIdEnergyV1 = "slab.semantic.operator.energy";
    inline constexpr auto COperatorIdResampleV1 = "slab.semantic.operator.resample";
    inline constexpr auto COperatorIdPlotFieldV1 = "slab.semantic.operator.plot_field";
    inline constexpr auto COperatorIdCoerceIntToRealV1 = "slab.semantic.operator.coerce_int_to_real";

    enum class ESemanticOperatorKindV1 : unsigned char {
        Transform,
        Reduction,
        Differential,
        Projection,
        Coercion,
        Visualization,
        SolverStep,
        Other
    };

    enum class ESemanticOperatorPropertyV1 : unsigned char {
        Linear = 0,
        Invertible = 1,
        Unitary = 2,
        SelfAdjoint = 3,
        Idempotent = 4,
        Local = 5,
        Global = 6
    };

    using FSemanticPropertyMaskV1 = std::uint64_t;

    inline auto SemanticPropertyMaskV1(const ESemanticOperatorPropertyV1 property) -> FSemanticPropertyMaskV1 {
        return static_cast<FSemanticPropertyMaskV1>(1ULL << static_cast<unsigned int>(property));
    }

    inline auto HasSemanticPropertyV1(const FSemanticPropertyMaskV1 mask, const ESemanticOperatorPropertyV1 property)
        -> bool {
        return (mask & SemanticPropertyMaskV1(property)) != 0;
    }

    struct FSemanticPortSchemaV1 {
        Str PortId;
        Str DisplayName;
        FSpaceId SpaceId;
        Str Latex;
        bool bRequired = true;
    };

    struct FSpaceSchemaV1 {
        FSpaceId SpaceId;
        Str DisplayName;
        Str CarrierTypeId;
        Vector<Str> Tags;
        std::map<Str, FReflectionValueV2> Attrs;
        Str Latex;
        Vector<Str> CompatibilityKeys;
        Vector<FSpaceId> CompatibleWith;
        Vector<FSpaceId> CoercibleTo;
    };

    struct FSemanticOperatorRelationsV1 {
        std::optional<FSemanticOperatorId> InverseOf;
        std::optional<FSemanticOperatorId> AdjointOf;
        Vector<FSemanticOperatorId> EquivalentTo;
    };

    struct FSemanticOperatorSchemaV1 {
        FSemanticOperatorId OperatorId;
        Str DisplayName;
        Str Description;
        Str LatexSymbol;
        Str LatexTemplate;
        Vector<FSemanticPortSchemaV1> DomainPorts;
        Vector<FSemanticPortSchemaV1> CodomainPorts;
        ESemanticOperatorKindV1 Kind = ESemanticOperatorKindV1::Other;
        FSemanticPropertyMaskV1 DeclaredProperties = 0;
        FSemanticOperatorRelationsV1 Relations;
        Vector<Str> Tags;
    };

    struct FImplementationBindingConstraintsV1 {
        Vector<Str> RequiredCarrierTypeIds;
        Vector<Str> RequiredTags;
        std::map<Str, FReflectionValueV2> RequiredAttrs;
    };

    struct FImplementationCostHintsV1 {
        Str ComplexityClass;
        Str LatencyClass;
    };

    struct FImplementationBindingV1 {
        FSemanticOperatorId SemanticOperatorId;
        Str TargetInterfaceId;
        Str TargetOperationId;
        FImplementationBindingConstraintsV1 Constraints;
        FImplementationCostHintsV1 CostHints;
    };

    enum class ESignatureMatchReasonV1 : unsigned char {
        ExactMatch,
        CompatibleButNeedsCoercion,
        Incompatible
    };

    struct FOperatorSignatureMatchResultV1 {
        FSemanticOperatorId OperatorId;
        ESignatureMatchReasonV1 Reason = ESignatureMatchReasonV1::Incompatible;
        Vector<Str> Diagnostics;
        Vector<FSemanticOperatorId> SuggestedCoercionOperatorIds;
    };

    class FSemanticCatalogV1 {
        Str CatalogVersion = CSemanticCatalogVersionV1;
        std::map<FSpaceId, FSpaceSchemaV1> SpacesById;
        std::map<FSemanticOperatorId, FSemanticOperatorSchemaV1> OperatorsById;
        std::map<FSemanticOperatorId, Vector<FImplementationBindingV1>> BindingsByOperatorId;
        bool bSeedLibraryRegistered = false;

        static auto ContainsId(const Vector<FSpaceId> &items, const FSpaceId &id) -> bool {
            return std::find(items.begin(), items.end(), id) != items.end();
        }

        [[nodiscard]] auto IsCompatibleSpace(const FSpaceId &from, const FSpaceId &to) const -> bool {
            if (from.empty() || to.empty()) return false;
            if (from == to) return true;

            const auto *fromSpace = FindSpaceById(from);
            if (fromSpace != nullptr && ContainsId(fromSpace->CompatibleWith, to)) return true;

            const auto *toSpace = FindSpaceById(to);
            if (toSpace != nullptr && ContainsId(toSpace->CompatibleWith, from)) return true;

            return false;
        }

        [[nodiscard]] auto IsDirectlyCoercible(const FSpaceId &from, const FSpaceId &to) const -> bool {
            if (from.empty() || to.empty()) return false;
            if (from == to) return true;

            const auto *fromSpace = FindSpaceById(from);
            if (fromSpace == nullptr) return false;
            return ContainsId(fromSpace->CoercibleTo, to);
        }

        [[nodiscard]] auto CanCoerceViaSingleHop(const FSpaceId &from, const FSpaceId &to) const -> bool {
            if (from.empty() || to.empty()) return false;
            if (IsDirectlyCoercible(from, to)) return true;

            const auto *fromSpace = FindSpaceById(from);
            if (fromSpace == nullptr) return false;

            for (const auto &mid : fromSpace->CoercibleTo) {
                if (IsDirectlyCoercible(mid, to)) return true;
                if (IsCompatibleSpace(mid, to)) return true;
            }

            return false;
        }

        [[nodiscard]] auto SuggestCoercionOperatorIds(const FSpaceId &from, const FSpaceId &to) const
            -> Vector<FSemanticOperatorId> {
            Vector<FSemanticOperatorId> suggestions;

            for (const auto &[operatorId, semanticOperator] : OperatorsById) {
                if (semanticOperator.DomainPorts.size() != 1 || semanticOperator.CodomainPorts.size() != 1) continue;
                if (semanticOperator.Kind != ESemanticOperatorKindV1::Coercion &&
                    semanticOperator.Kind != ESemanticOperatorKindV1::Projection) {
                    continue;
                }

                if (semanticOperator.DomainPorts.front().SpaceId != from) continue;
                if (semanticOperator.CodomainPorts.front().SpaceId != to) continue;
                suggestions.push_back(operatorId);
            }

            return suggestions;
        }

        [[nodiscard]] auto FormatMismatchDiagnostic(const Str &kind,
                                                    const std::size_t index,
                                                    const FSpaceId &provided,
                                                    const FSpaceId &required) const -> Str {
            return kind + "[" + ToStr(index) + "] '" + provided + "' -> '" + required + "'";
        }

        [[nodiscard]] auto BuildMatchResultForOperator(const FSemanticOperatorSchemaV1 &semanticOperator,
                                                       const Vector<FSpaceId> &domainSpaces,
                                                       const Vector<FSpaceId> &codomainSpaces) const
            -> FOperatorSignatureMatchResultV1 {
            FOperatorSignatureMatchResultV1 result;
            result.OperatorId = semanticOperator.OperatorId;

            bool bNeedsCoercion = false;

            if (semanticOperator.DomainPorts.size() != domainSpaces.size()) {
                result.Reason = ESignatureMatchReasonV1::Incompatible;
                result.Diagnostics.push_back(
                    "domain arity mismatch: expected " + ToStr(semanticOperator.DomainPorts.size()) +
                    ", got " + ToStr(domainSpaces.size()));
                return result;
            }

            if (semanticOperator.CodomainPorts.size() != codomainSpaces.size()) {
                result.Reason = ESignatureMatchReasonV1::Incompatible;
                result.Diagnostics.push_back(
                    "codomain arity mismatch: expected " + ToStr(semanticOperator.CodomainPorts.size()) +
                    ", got " + ToStr(codomainSpaces.size()));
                return result;
            }

            for (std::size_t i = 0; i < semanticOperator.DomainPorts.size(); ++i) {
                const auto &providedSpace = domainSpaces[i];
                const auto &requiredSpace = semanticOperator.DomainPorts[i].SpaceId;
                if (providedSpace == requiredSpace) continue;

                if (IsCompatibleSpace(providedSpace, requiredSpace) || CanCoerceViaSingleHop(providedSpace, requiredSpace)) {
                    bNeedsCoercion = true;
                    result.Diagnostics.push_back(
                        "domain requires coercion: " +
                        FormatMismatchDiagnostic("domain", i, providedSpace, requiredSpace));

                    const auto suggestions = SuggestCoercionOperatorIds(providedSpace, requiredSpace);
                    result.SuggestedCoercionOperatorIds.insert(
                        result.SuggestedCoercionOperatorIds.end(),
                        suggestions.begin(),
                        suggestions.end());
                    continue;
                }

                result.Reason = ESignatureMatchReasonV1::Incompatible;
                result.Diagnostics.push_back(
                    "domain incompatible: " +
                    FormatMismatchDiagnostic("domain", i, providedSpace, requiredSpace));
                return result;
            }

            for (std::size_t i = 0; i < semanticOperator.CodomainPorts.size(); ++i) {
                const auto &providedSpace = semanticOperator.CodomainPorts[i].SpaceId;
                const auto &requiredSpace = codomainSpaces[i];
                if (providedSpace == requiredSpace) continue;

                if (IsCompatibleSpace(providedSpace, requiredSpace) || CanCoerceViaSingleHop(providedSpace, requiredSpace)) {
                    bNeedsCoercion = true;
                    result.Diagnostics.push_back(
                        "codomain requires coercion: " +
                        FormatMismatchDiagnostic("codomain", i, providedSpace, requiredSpace));

                    const auto suggestions = SuggestCoercionOperatorIds(providedSpace, requiredSpace);
                    result.SuggestedCoercionOperatorIds.insert(
                        result.SuggestedCoercionOperatorIds.end(),
                        suggestions.begin(),
                        suggestions.end());
                    continue;
                }

                result.Reason = ESignatureMatchReasonV1::Incompatible;
                result.Diagnostics.push_back(
                    "codomain incompatible: " +
                    FormatMismatchDiagnostic("codomain", i, providedSpace, requiredSpace));
                return result;
            }

            if (bNeedsCoercion) {
                result.Reason = ESignatureMatchReasonV1::CompatibleButNeedsCoercion;
            } else {
                result.Reason = ESignatureMatchReasonV1::ExactMatch;
            }

            std::sort(result.SuggestedCoercionOperatorIds.begin(), result.SuggestedCoercionOperatorIds.end());
            result.SuggestedCoercionOperatorIds.erase(
                std::unique(result.SuggestedCoercionOperatorIds.begin(), result.SuggestedCoercionOperatorIds.end()),
                result.SuggestedCoercionOperatorIds.end());

            return result;
        }

        static auto AddUnique(Vector<FSpaceId> &spaces, const FSpaceId &spaceId) -> void {
            if (spaceId.empty()) return;
            if (ContainsId(spaces, spaceId)) return;
            spaces.push_back(spaceId);
        }

    public:
        [[nodiscard]] auto GetCatalogVersion() const -> const Str & { return CatalogVersion; }

        auto SetCatalogVersion(Str version) -> void { CatalogVersion = std::move(version); }

        auto RegisterSpace(FSpaceSchemaV1 space) -> bool {
            if (space.SpaceId.empty()) return false;
            if (space.DisplayName.empty()) space.DisplayName = space.SpaceId;

            SpacesById[space.SpaceId] = std::move(space);
            return true;
        }

        auto RegisterCompatibility(const FSpaceId &spaceA, const FSpaceId &spaceB, const bool bBidirectional = true)
            -> bool {
            if (spaceA.empty() || spaceB.empty()) return false;

            auto *a = FindSpaceById(spaceA);
            auto *b = FindSpaceById(spaceB);
            if (a == nullptr || b == nullptr) return false;

            AddUnique(a->CompatibleWith, spaceB);
            if (bBidirectional) AddUnique(b->CompatibleWith, spaceA);
            return true;
        }

        auto RegisterCoercion(const FSpaceId &from, const FSpaceId &to) -> bool {
            if (from.empty() || to.empty()) return false;

            auto *source = FindSpaceById(from);
            auto *target = FindSpaceById(to);
            if (source == nullptr || target == nullptr) return false;

            AddUnique(source->CoercibleTo, to);
            return true;
        }

        auto RegisterOperator(FSemanticOperatorSchemaV1 semanticOperator) -> bool {
            if (semanticOperator.OperatorId.empty()) return false;
            if (semanticOperator.DisplayName.empty()) semanticOperator.DisplayName = semanticOperator.OperatorId;

            OperatorsById[semanticOperator.OperatorId] = std::move(semanticOperator);
            return true;
        }

        auto RegisterBinding(FImplementationBindingV1 binding) -> bool {
            if (binding.SemanticOperatorId.empty()) return false;
            if (binding.TargetInterfaceId.empty()) return false;
            if (binding.TargetOperationId.empty()) return false;

            auto &bindings = BindingsByOperatorId[binding.SemanticOperatorId];
            const auto it = std::find_if(bindings.begin(), bindings.end(), [&](const auto &existing) {
                return existing.SemanticOperatorId == binding.SemanticOperatorId &&
                       existing.TargetInterfaceId == binding.TargetInterfaceId &&
                       existing.TargetOperationId == binding.TargetOperationId;
            });

            if (it != bindings.end()) {
                *it = std::move(binding);
                return true;
            }

            bindings.push_back(std::move(binding));
            return true;
        }

        auto ClearBindings() -> void { BindingsByOperatorId.clear(); }

        auto Clear() -> void {
            SpacesById.clear();
            OperatorsById.clear();
            BindingsByOperatorId.clear();
            bSeedLibraryRegistered = false;
        }

        [[nodiscard]] auto FindSpaceById(const FSpaceId &spaceId) -> FSpaceSchemaV1 * {
            const auto it = SpacesById.find(spaceId);
            if (it == SpacesById.end()) return nullptr;
            return &it->second;
        }

        [[nodiscard]] auto FindSpaceById(const FSpaceId &spaceId) const -> const FSpaceSchemaV1 * {
            const auto it = SpacesById.find(spaceId);
            if (it == SpacesById.end()) return nullptr;
            return &it->second;
        }

        [[nodiscard]] auto FindOperatorById(const FSemanticOperatorId &operatorId) -> FSemanticOperatorSchemaV1 * {
            const auto it = OperatorsById.find(operatorId);
            if (it == OperatorsById.end()) return nullptr;
            return &it->second;
        }

        [[nodiscard]] auto FindOperatorById(const FSemanticOperatorId &operatorId) const
            -> const FSemanticOperatorSchemaV1 * {
            const auto it = OperatorsById.find(operatorId);
            if (it == OperatorsById.end()) return nullptr;
            return &it->second;
        }

        [[nodiscard]] auto ListSpaces() const -> Vector<const FSpaceSchemaV1 *> {
            Vector<const FSpaceSchemaV1 *> spaces;
            spaces.reserve(SpacesById.size());
            for (const auto &[spaceId, space] : SpacesById) {
                (void) spaceId;
                spaces.push_back(&space);
            }

            std::sort(spaces.begin(), spaces.end(), [](const auto *lhs, const auto *rhs) {
                if (lhs == nullptr || rhs == nullptr) return lhs < rhs;
                if (lhs->DisplayName == rhs->DisplayName) return lhs->SpaceId < rhs->SpaceId;
                return lhs->DisplayName < rhs->DisplayName;
            });

            return spaces;
        }

        [[nodiscard]] auto ListOperators() const -> Vector<const FSemanticOperatorSchemaV1 *> {
            Vector<const FSemanticOperatorSchemaV1 *> operators;
            operators.reserve(OperatorsById.size());
            for (const auto &[operatorId, semanticOperator] : OperatorsById) {
                (void) operatorId;
                operators.push_back(&semanticOperator);
            }

            std::sort(operators.begin(), operators.end(), [](const auto *lhs, const auto *rhs) {
                if (lhs == nullptr || rhs == nullptr) return lhs < rhs;
                if (lhs->DisplayName == rhs->DisplayName) return lhs->OperatorId < rhs->OperatorId;
                return lhs->DisplayName < rhs->DisplayName;
            });

            return operators;
        }

        [[nodiscard]] auto QueryOperatorsForSignature(const Vector<FSpaceId> &domainSpaces,
                                                      const Vector<FSpaceId> &codomainSpaces) const
            -> Vector<const FSemanticOperatorSchemaV1 *> {
            // Strict-by-default: only exact semantic-space matches.
            Vector<const FSemanticOperatorSchemaV1 *> matches;

            for (const auto &[operatorId, semanticOperator] : OperatorsById) {
                (void) operatorId;
                const auto match = BuildMatchResultForOperator(semanticOperator, domainSpaces, codomainSpaces);
                if (match.Reason == ESignatureMatchReasonV1::ExactMatch) matches.push_back(&semanticOperator);
            }

            return matches;
        }

        [[nodiscard]] auto QueryOperatorMatchesForSignature(const Vector<FSpaceId> &domainSpaces,
                                                            const Vector<FSpaceId> &codomainSpaces) const
            -> Vector<FOperatorSignatureMatchResultV1> {
            Vector<FOperatorSignatureMatchResultV1> matches;
            matches.reserve(OperatorsById.size());

            for (const auto &[operatorId, semanticOperator] : OperatorsById) {
                (void) operatorId;
                matches.push_back(BuildMatchResultForOperator(semanticOperator, domainSpaces, codomainSpaces));
            }

            std::stable_sort(matches.begin(), matches.end(), [](const auto &lhs, const auto &rhs) {
                return static_cast<int>(lhs.Reason) < static_cast<int>(rhs.Reason);
            });

            return matches;
        }

        [[nodiscard]] auto QueryBindingsForOperator(const FSemanticOperatorId &operatorId) const
            -> Vector<FImplementationBindingV1> {
            const auto it = BindingsByOperatorId.find(operatorId);
            if (it == BindingsByOperatorId.end()) return {};
            return it->second;
        }

        [[nodiscard]] auto QuerySuggestedCoercions(const FSpaceId &fromSpaceId, const FSpaceId &toSpaceId) const
            -> Vector<FSpaceId> {
            if (fromSpaceId.empty() || toSpaceId.empty()) return {};
            if (fromSpaceId == toSpaceId) return {fromSpaceId};
            if (IsCompatibleSpace(fromSpaceId, toSpaceId) || IsDirectlyCoercible(fromSpaceId, toSpaceId)) {
                return {fromSpaceId, toSpaceId};
            }

            const auto *source = FindSpaceById(fromSpaceId);
            if (source == nullptr) return {};

            for (const auto &midSpace : source->CoercibleTo) {
                if (midSpace == toSpaceId) return {fromSpaceId, toSpaceId};
                if (IsCompatibleSpace(midSpace, toSpaceId) || IsDirectlyCoercible(midSpace, toSpaceId)) {
                    return {fromSpaceId, midSpace, toSpaceId};
                }
            }

            return {};
        }

        [[nodiscard]] auto IsSeedLibraryRegistered() const -> bool { return bSeedLibraryRegistered; }

        auto SetSeedLibraryRegistered(const bool value) -> void { bSeedLibraryRegistered = value; }
    };

    inline auto MakeSemanticPortSchemaV1(Str portId,
                                         Str displayName,
                                         FSpaceId spaceId,
                                         Str latex = {},
                                         const bool bRequired = true) -> FSemanticPortSchemaV1 {
        FSemanticPortSchemaV1 port;
        port.PortId = std::move(portId);
        port.DisplayName = std::move(displayName);
        port.SpaceId = std::move(spaceId);
        port.Latex = std::move(latex);
        port.bRequired = bRequired;
        return port;
    }

    inline auto RegisterSemanticSeedLibraryV1(FSemanticCatalogV1 &catalog) -> void {
        {
            FSpaceSchemaV1 space;
            space.SpaceId = CSpaceIdScalarRealV1;
            space.DisplayName = "Scalar Real";
            space.CarrierTypeId = CTypeIdScalarFloat64;
            space.Tags = {"scalar", "real"};
            space.Latex = "\\mathbb{R}";
            catalog.RegisterSpace(std::move(space));
        }
        {
            FSpaceSchemaV1 space;
            space.SpaceId = CSpaceIdScalarIntV1;
            space.DisplayName = "Scalar Int";
            space.CarrierTypeId = CTypeIdScalarInt32; // int64 carrier is not available yet in Reflection V2.
            space.Tags = {"scalar", "integer"};
            space.Latex = "\\mathbb{Z}";
            catalog.RegisterSpace(std::move(space));
        }
        {
            FSpaceSchemaV1 space;
            space.SpaceId = CSpaceIdBoolV1;
            space.DisplayName = "Bool";
            space.CarrierTypeId = CTypeIdScalarBool;
            space.Tags = {"scalar", "bool"};
            space.Latex = "\\mathbb{B}";
            catalog.RegisterSpace(std::move(space));
        }
        {
            FSpaceSchemaV1 space;
            space.SpaceId = CSpaceIdVectorRealV1;
            space.DisplayName = "Vector Real";
            space.CarrierTypeId = CTypeIdVectorRealOpaqueV1;
            space.Tags = {"vector", "real"};
            space.Latex = "\\mathbb{R}^n";
            space.Attrs.emplace("carrier.todo", MakeStringValue("replace_with_stable_vector_type_id"));
            catalog.RegisterSpace(std::move(space));
        }
        {
            FSpaceSchemaV1 space;
            space.SpaceId = CSpaceIdField2DRealV1;
            space.DisplayName = "Field 2D Real";
            space.CarrierTypeId = CTypeIdField2DRealOpaqueV1;
            space.Tags = {"field", "2d", "real"};
            space.Latex = "f: \\mathbb{R}^2 \\to \\mathbb{R}";
            space.Attrs.emplace("carrier.todo", MakeStringValue("replace_with_stable_field_type_id"));
            catalog.RegisterSpace(std::move(space));
        }
        {
            FSpaceSchemaV1 space;
            space.SpaceId = CSpaceIdSpectrum1DComplexV1;
            space.DisplayName = "Spectrum 1D Complex";
            space.CarrierTypeId = CTypeIdSpectrum1DComplexOpaqueV1;
            space.Tags = {"spectrum", "1d", "complex"};
            space.Latex = "\\hat{f}(k)";
            space.Attrs.emplace("carrier.todo", MakeStringValue("replace_with_stable_complex_spectrum_type_id"));
            catalog.RegisterSpace(std::move(space));
        }
        {
            FSpaceSchemaV1 space;
            space.SpaceId = CSpaceIdFunctionRtoRV1;
            space.DisplayName = "Function R->R";
            space.CarrierTypeId = CTypeIdFunctionRtoR;
            space.Tags = {"function", "real", "analytic_or_sampled"};
            space.Latex = "f: \\mathbb{R} \\to \\mathbb{R}";
            catalog.RegisterSpace(std::move(space));
        }
        {
            FSpaceSchemaV1 space;
            space.SpaceId = CSpaceIdVisualizationHandleV1;
            space.DisplayName = "Visualization Handle";
            space.CarrierTypeId = CTypeIdVisualizationHandleOpaqueV1;
            space.Tags = {"visualization", "handle"};
            space.Latex = "\\mathcal{V}";
            space.Attrs.emplace("carrier.todo", MakeStringValue("replace_with_stable_visualization_handle_type_id"));
            catalog.RegisterSpace(std::move(space));
        }
        {
            FSpaceSchemaV1 space;
            space.SpaceId = CSpaceIdUnitV1;
            space.DisplayName = "Unit";
            space.CarrierTypeId = CTypeIdUnitOpaqueV1;
            space.Tags = {"unit"};
            space.Latex = "\\mathbf{1}";
            catalog.RegisterSpace(std::move(space));
        }

        (void) catalog.RegisterCompatibility(CSpaceIdScalarRealV1, CSpaceIdScalarIntV1, true);
        (void) catalog.RegisterCoercion(CSpaceIdScalarIntV1, CSpaceIdScalarRealV1);
        (void) catalog.RegisterCoercion(CSpaceIdField2DRealV1, CSpaceIdVectorRealV1);
        (void) catalog.RegisterCoercion(CSpaceIdField2DRealV1, CSpaceIdVisualizationHandleV1);
        (void) catalog.RegisterCoercion(CSpaceIdSpectrum1DComplexV1, CSpaceIdField2DRealV1);

        {
            FSemanticOperatorSchemaV1 semanticOperator;
            semanticOperator.OperatorId = COperatorIdIdentityV1;
            semanticOperator.DisplayName = "Identity";
            semanticOperator.Description = "Identity map in function space.";
            semanticOperator.LatexSymbol = "\\mathrm{Id}";
            semanticOperator.LatexTemplate = "f \\mapsto f";
            semanticOperator.Kind = ESemanticOperatorKindV1::Transform;
            semanticOperator.DeclaredProperties =
                SemanticPropertyMaskV1(ESemanticOperatorPropertyV1::Linear) |
                SemanticPropertyMaskV1(ESemanticOperatorPropertyV1::Invertible) |
                SemanticPropertyMaskV1(ESemanticOperatorPropertyV1::Idempotent);
            semanticOperator.DomainPorts = {
                MakeSemanticPortSchemaV1("input", "Input", CSpaceIdFunctionRtoRV1, "f")
            };
            semanticOperator.CodomainPorts = {
                MakeSemanticPortSchemaV1("output", "Output", CSpaceIdFunctionRtoRV1, "f")
            };
            catalog.RegisterOperator(std::move(semanticOperator));
        }
        {
            FSemanticOperatorSchemaV1 semanticOperator;
            semanticOperator.OperatorId = COperatorIdFourierTransformV1;
            semanticOperator.DisplayName = "Fourier Transform";
            semanticOperator.Description = "Frequency-domain transform.";
            semanticOperator.LatexSymbol = "\\mathcal{F}";
            semanticOperator.LatexTemplate = "\\hat{f} = \\mathcal{F}(f)";
            semanticOperator.Kind = ESemanticOperatorKindV1::Transform;
            semanticOperator.DeclaredProperties = SemanticPropertyMaskV1(ESemanticOperatorPropertyV1::Linear);
            semanticOperator.DomainPorts = {
                MakeSemanticPortSchemaV1("field", "Field", CSpaceIdField2DRealV1, "f")
            };
            semanticOperator.CodomainPorts = {
                MakeSemanticPortSchemaV1("spectrum", "Spectrum", CSpaceIdSpectrum1DComplexV1, "\\hat{f}")
            };
            catalog.RegisterOperator(std::move(semanticOperator));
        }
        {
            FSemanticOperatorSchemaV1 semanticOperator;
            semanticOperator.OperatorId = COperatorIdInverseFourierTransformV1;
            semanticOperator.DisplayName = "Inverse Fourier Transform";
            semanticOperator.Description = "Inverse frequency-domain transform.";
            semanticOperator.LatexSymbol = "\\mathcal{F}^{-1}";
            semanticOperator.LatexTemplate = "f = \\mathcal{F}^{-1}(\\hat{f})";
            semanticOperator.Kind = ESemanticOperatorKindV1::Transform;
            semanticOperator.DeclaredProperties = SemanticPropertyMaskV1(ESemanticOperatorPropertyV1::Linear);
            semanticOperator.Relations.InverseOf = COperatorIdFourierTransformV1;
            semanticOperator.DomainPorts = {
                MakeSemanticPortSchemaV1("spectrum", "Spectrum", CSpaceIdSpectrum1DComplexV1, "\\hat{f}")
            };
            semanticOperator.CodomainPorts = {
                MakeSemanticPortSchemaV1("field", "Field", CSpaceIdField2DRealV1, "f")
            };
            catalog.RegisterOperator(std::move(semanticOperator));
        }
        {
            FSemanticOperatorSchemaV1 semanticOperator;
            semanticOperator.OperatorId = COperatorIdLaplacianV1;
            semanticOperator.DisplayName = "Laplacian";
            semanticOperator.Description = "Second-order differential operator.";
            semanticOperator.LatexSymbol = "\\Delta";
            semanticOperator.LatexTemplate = "\\Delta f";
            semanticOperator.Kind = ESemanticOperatorKindV1::Differential;
            semanticOperator.DeclaredProperties =
                SemanticPropertyMaskV1(ESemanticOperatorPropertyV1::Linear) |
                SemanticPropertyMaskV1(ESemanticOperatorPropertyV1::Local);
            semanticOperator.DomainPorts = {
                MakeSemanticPortSchemaV1("field", "Field", CSpaceIdField2DRealV1, "f")
            };
            semanticOperator.CodomainPorts = {
                MakeSemanticPortSchemaV1("laplacian", "Laplacian", CSpaceIdField2DRealV1, "\\Delta f")
            };
            catalog.RegisterOperator(std::move(semanticOperator));
        }
        {
            FSemanticOperatorSchemaV1 semanticOperator;
            semanticOperator.OperatorId = COperatorIdGradientV1;
            semanticOperator.DisplayName = "Gradient";
            semanticOperator.Description = "First-order differential operator.";
            semanticOperator.LatexSymbol = "\\nabla";
            semanticOperator.LatexTemplate = "\\nabla f";
            semanticOperator.Kind = ESemanticOperatorKindV1::Differential;
            semanticOperator.DeclaredProperties =
                SemanticPropertyMaskV1(ESemanticOperatorPropertyV1::Linear) |
                SemanticPropertyMaskV1(ESemanticOperatorPropertyV1::Local);
            semanticOperator.DomainPorts = {
                MakeSemanticPortSchemaV1("field", "Field", CSpaceIdField2DRealV1, "f")
            };
            semanticOperator.CodomainPorts = {
                MakeSemanticPortSchemaV1("gradient", "Gradient", CSpaceIdVectorRealV1, "\\nabla f")
            };
            semanticOperator.Tags = {"todo.vector_field_representation"};
            catalog.RegisterOperator(std::move(semanticOperator));
        }
        {
            FSemanticOperatorSchemaV1 semanticOperator;
            semanticOperator.OperatorId = COperatorIdAverageV1;
            semanticOperator.DisplayName = "Average";
            semanticOperator.Description = "Field mean value.";
            semanticOperator.LatexSymbol = "\\langle \\cdot \\rangle";
            semanticOperator.LatexTemplate = "\\langle f \\rangle";
            semanticOperator.Kind = ESemanticOperatorKindV1::Reduction;
            semanticOperator.DomainPorts = {
                MakeSemanticPortSchemaV1("field", "Field", CSpaceIdField2DRealV1, "f")
            };
            semanticOperator.CodomainPorts = {
                MakeSemanticPortSchemaV1("average", "Average", CSpaceIdScalarRealV1, "\\langle f \\rangle")
            };
            catalog.RegisterOperator(std::move(semanticOperator));
        }
        {
            FSemanticOperatorSchemaV1 semanticOperator;
            semanticOperator.OperatorId = COperatorIdEnergyV1;
            semanticOperator.DisplayName = "Energy";
            semanticOperator.Description = "Energy functional.";
            semanticOperator.LatexSymbol = "\\mathcal{E}";
            semanticOperator.LatexTemplate = "\\mathcal{E}(f)";
            semanticOperator.Kind = ESemanticOperatorKindV1::Reduction;
            semanticOperator.DomainPorts = {
                MakeSemanticPortSchemaV1("field", "Field", CSpaceIdField2DRealV1, "f")
            };
            semanticOperator.CodomainPorts = {
                MakeSemanticPortSchemaV1("energy", "Energy", CSpaceIdScalarRealV1, "\\mathcal{E}")
            };
            catalog.RegisterOperator(std::move(semanticOperator));
        }
        {
            FSemanticOperatorSchemaV1 semanticOperator;
            semanticOperator.OperatorId = COperatorIdResampleV1;
            semanticOperator.DisplayName = "Resample";
            semanticOperator.Description = "Resampling/projection operator.";
            semanticOperator.LatexSymbol = "\\mathcal{R}";
            semanticOperator.LatexTemplate = "f' = \\mathcal{R}(f)";
            semanticOperator.Kind = ESemanticOperatorKindV1::Projection;
            semanticOperator.DomainPorts = {
                MakeSemanticPortSchemaV1("field", "Field", CSpaceIdField2DRealV1, "f")
            };
            semanticOperator.CodomainPorts = {
                MakeSemanticPortSchemaV1("resampled", "Resampled", CSpaceIdField2DRealV1, "f'")
            };
            catalog.RegisterOperator(std::move(semanticOperator));
        }
        {
            FSemanticOperatorSchemaV1 semanticOperator;
            semanticOperator.OperatorId = COperatorIdCoerceIntToRealV1;
            semanticOperator.DisplayName = "Coerce Int->Real";
            semanticOperator.Description = "Lossless scalar widening from integer to real.";
            semanticOperator.LatexSymbol = "\\iota";
            semanticOperator.LatexTemplate = "\\iota: \\mathbb{Z} \\hookrightarrow \\mathbb{R}";
            semanticOperator.Kind = ESemanticOperatorKindV1::Coercion;
            semanticOperator.DomainPorts = {
                MakeSemanticPortSchemaV1("value", "Value", CSpaceIdScalarIntV1, "n")
            };
            semanticOperator.CodomainPorts = {
                MakeSemanticPortSchemaV1("value_real", "ValueReal", CSpaceIdScalarRealV1, "\\iota(n)")
            };
            catalog.RegisterOperator(std::move(semanticOperator));
        }
        {
            FSemanticOperatorSchemaV1 semanticOperator;
            semanticOperator.OperatorId = COperatorIdPlotFieldV1;
            semanticOperator.DisplayName = "Plot Field";
            semanticOperator.Description = "Visualization mapping.";
            semanticOperator.LatexSymbol = "\\mathcal{P}";
            semanticOperator.LatexTemplate = "v = \\mathcal{P}(f)";
            semanticOperator.Kind = ESemanticOperatorKindV1::Visualization;
            semanticOperator.DomainPorts = {
                MakeSemanticPortSchemaV1("field", "Field", CSpaceIdField2DRealV1, "f")
            };
            semanticOperator.CodomainPorts = {
                MakeSemanticPortSchemaV1("viz", "Visualization", CSpaceIdVisualizationHandleV1, "v")
            };
            catalog.RegisterOperator(std::move(semanticOperator));
        }

        auto *fourier = catalog.FindOperatorById(COperatorIdFourierTransformV1);
        if (fourier != nullptr) fourier->Relations.InverseOf = COperatorIdInverseFourierTransformV1;
    }

    inline auto GetSemanticCatalogV1() -> FSemanticCatalogV1 & {
        static FSemanticCatalogV1 catalog;
        return catalog;
    }

    inline auto InitSemanticCatalogV1() -> void {
        auto &catalog = GetSemanticCatalogV1();
        if (catalog.IsSeedLibraryRegistered()) return;

        RegisterSemanticSeedLibraryV1(catalog);
        catalog.SetSeedLibraryRegistered(true);
    }

} // namespace Slab::Core::Reflection::V2

#endif // STUDIOSLAB_SEMANTIC_TYPES_V1_H
