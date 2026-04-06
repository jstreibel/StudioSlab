#ifndef STUDIOSLAB_REFLECTION_SEMANTICS_OVERLAY_V1_H
#define STUDIOSLAB_REFLECTION_SEMANTICS_OVERLAY_V1_H

#include "SemanticTypesV1.h"

#include <map>
#include <optional>
#include <tuple>

namespace Slab::Core::Reflection::V2 {

    struct FParameterSemanticKeyV1 {
        Str InterfaceId;
        Str ParameterId;

        [[nodiscard]] auto operator<(const FParameterSemanticKeyV1 &other) const -> bool {
            return std::tie(InterfaceId, ParameterId) < std::tie(other.InterfaceId, other.ParameterId);
        }
    };

    struct FOperationPortSemanticKeyV1 {
        Str InterfaceId;
        Str OperationId;
        Str PortId;

        [[nodiscard]] auto operator<(const FOperationPortSemanticKeyV1 &other) const -> bool {
            return std::tie(InterfaceId, OperationId, PortId) <
                   std::tie(other.InterfaceId, other.OperationId, other.PortId);
        }
    };

    struct FOperationSemanticKeyV1 {
        Str InterfaceId;
        Str OperationId;

        [[nodiscard]] auto operator<(const FOperationSemanticKeyV1 &other) const -> bool {
            return std::tie(InterfaceId, OperationId) < std::tie(other.InterfaceId, other.OperationId);
        }
    };

    using FPortSpaceBindingV1 = Pair<Str, FSpaceId>;

    struct FAttachSemanticsOptionsV1 {
        bool bAttachImplementationBindings = false;
        bool bIncludeConfigurationOperationBindings = false;
    };

    class FReflectionSemanticsOverlayV1 {
    public:
        std::map<FParameterSemanticKeyV1, FSpaceId> ParameterSemantics;
        std::map<FOperationPortSemanticKeyV1, FSpaceId> OperationInputPortSemantics;
        std::map<FOperationPortSemanticKeyV1, FSpaceId> OperationOutputPortSemantics;
        std::map<FOperationSemanticKeyV1, FSemanticOperatorId> OperationSemanticOperatorId;

        auto Clear() -> void {
            ParameterSemantics.clear();
            OperationInputPortSemantics.clear();
            OperationOutputPortSemantics.clear();
            OperationSemanticOperatorId.clear();
        }

        auto AttachParameterSpace(const Str &interfaceId, const Str &parameterId, const FSpaceId &spaceId) -> bool {
            if (interfaceId.empty() || parameterId.empty() || spaceId.empty()) return false;
            ParameterSemantics[FParameterSemanticKeyV1{interfaceId, parameterId}] = spaceId;
            return true;
        }

        auto AttachOperationPortSpaces(const Str &interfaceId,
                                       const Str &operationId,
                                       const Vector<FPortSpaceBindingV1> &inputSpaces,
                                       const Vector<FPortSpaceBindingV1> &outputSpaces) -> bool {
            if (interfaceId.empty() || operationId.empty()) return false;

            for (const auto &[portId, spaceId] : inputSpaces) {
                if (portId.empty() || spaceId.empty()) continue;
                OperationInputPortSemantics[FOperationPortSemanticKeyV1{interfaceId, operationId, portId}] = spaceId;
            }

            for (const auto &[portId, spaceId] : outputSpaces) {
                if (portId.empty() || spaceId.empty()) continue;
                OperationOutputPortSemantics[FOperationPortSemanticKeyV1{interfaceId, operationId, portId}] = spaceId;
            }

            return true;
        }

        // Semantic operator is abstract meaning; binding points to concrete reflected operation.
        auto AttachOperationAsImplementationOf(const Str &interfaceId,
                                               const Str &operationId,
                                               const FSemanticOperatorId &semanticOperatorId,
                                               const FImplementationBindingConstraintsV1 &constraints = {},
                                               const FImplementationCostHintsV1 &costHints = {},
                                               FSemanticCatalogV1 *pSemanticCatalog = nullptr) -> bool {
            if (interfaceId.empty() || operationId.empty() || semanticOperatorId.empty()) return false;

            OperationSemanticOperatorId[FOperationSemanticKeyV1{interfaceId, operationId}] = semanticOperatorId;

            if (pSemanticCatalog == nullptr) return true;

            FImplementationBindingV1 binding;
            binding.SemanticOperatorId = semanticOperatorId;
            binding.TargetInterfaceId = interfaceId;
            binding.TargetOperationId = operationId;
            binding.Constraints = constraints;
            binding.CostHints = costHints;
            return pSemanticCatalog->RegisterBinding(std::move(binding));
        }

        [[nodiscard]] auto FindParameterSpace(const Str &interfaceId, const Str &parameterId) const
            -> std::optional<FSpaceId> {
            const auto it = ParameterSemantics.find(FParameterSemanticKeyV1{interfaceId, parameterId});
            if (it == ParameterSemantics.end()) return std::nullopt;
            return it->second;
        }

        [[nodiscard]] auto FindOperationInputPortSpace(const Str &interfaceId,
                                                       const Str &operationId,
                                                       const Str &portId) const -> std::optional<FSpaceId> {
            const auto it = OperationInputPortSemantics.find(FOperationPortSemanticKeyV1{interfaceId, operationId, portId});
            if (it == OperationInputPortSemantics.end()) return std::nullopt;
            return it->second;
        }

        [[nodiscard]] auto FindOperationOutputPortSpace(const Str &interfaceId,
                                                        const Str &operationId,
                                                        const Str &portId) const -> std::optional<FSpaceId> {
            const auto it = OperationOutputPortSemantics.find(FOperationPortSemanticKeyV1{interfaceId, operationId, portId});
            if (it == OperationOutputPortSemantics.end()) return std::nullopt;
            return it->second;
        }

        [[nodiscard]] auto FindOperationSemanticOperatorId(const Str &interfaceId, const Str &operationId) const
            -> std::optional<FSemanticOperatorId> {
            const auto it = OperationSemanticOperatorId.find(FOperationSemanticKeyV1{interfaceId, operationId});
            if (it == OperationSemanticOperatorId.end()) return std::nullopt;
            return it->second;
        }
    };

    inline auto GuessSemanticSpaceByTypeIdV1(const Str &typeId) -> std::optional<FSpaceId> {
        if (typeId.empty()) return std::nullopt;

        if (typeId == CTypeIdScalarFloat64) return CSpaceIdScalarRealV1;
        if (typeId == CTypeIdScalarInt32) return CSpaceIdScalarIntV1;
        if (typeId == CTypeIdScalarBool) return CSpaceIdBoolV1;
        if (typeId == CTypeIdFunctionRtoR) return CSpaceIdFunctionRtoRV1;
        if (typeId == CTypeIdFunctionR2toR) return CSpaceIdField2DRealV1;

        return std::nullopt;
    }

    inline auto AttachSemanticsForCatalogV1(const FReflectionCatalogV2 &catalog,
                                            FReflectionSemanticsOverlayV1 &overlay,
                                            FSemanticCatalogV1 &semanticCatalog,
                                            const FAttachSemanticsOptionsV1 &options = {}) -> void {
        const auto IsConfigurationOperation = [](const Str &operationId) -> bool {
            if (operationId.starts_with("command.parameter.")) return true;
            if (operationId.starts_with("command.artist.set_")) return true;
            if (operationId == "command.artist.double_sample_count") return true;
            return false;
        };

        for (const auto &interfaceSchema : catalog.Interfaces) {
            if (interfaceSchema.InterfaceId.empty()) continue;

            for (const auto &parameterSchema : interfaceSchema.Parameters) {
                if (parameterSchema.ParameterId.empty()) continue;
                const auto spaceId = GuessSemanticSpaceByTypeIdV1(parameterSchema.TypeId);
                if (!spaceId.has_value()) continue;
                (void) overlay.AttachParameterSpace(interfaceSchema.InterfaceId, parameterSchema.ParameterId, *spaceId);
            }

            for (const auto &operationSchema : interfaceSchema.Operations) {
                if (operationSchema.OperationId.empty()) continue;

                Vector<FPortSpaceBindingV1> inputSpaces;
                Vector<FPortSpaceBindingV1> outputSpaces;

                for (const auto &fieldSchema : operationSchema.Inputs) {
                    if (fieldSchema.FieldId.empty()) continue;
                    const auto spaceId = GuessSemanticSpaceByTypeIdV1(fieldSchema.TypeId);
                    if (!spaceId.has_value()) continue;
                    inputSpaces.push_back({fieldSchema.FieldId, *spaceId});
                }

                for (const auto &fieldSchema : operationSchema.Outputs) {
                    if (fieldSchema.FieldId.empty()) continue;
                    const auto spaceId = GuessSemanticSpaceByTypeIdV1(fieldSchema.TypeId);
                    if (!spaceId.has_value()) continue;
                    outputSpaces.push_back({fieldSchema.FieldId, *spaceId});
                }

                (void) overlay.AttachOperationPortSpaces(
                    interfaceSchema.InterfaceId,
                    operationSchema.OperationId,
                    inputSpaces,
                    outputSpaces);

                if (!options.bAttachImplementationBindings) continue;
                if (!options.bIncludeConfigurationOperationBindings &&
                    IsConfigurationOperation(operationSchema.OperationId)) {
                    continue;
                }

                if (operationSchema.OperationId == "command.artist.set_function") {
                    FImplementationBindingConstraintsV1 constraints;
                    constraints.RequiredCarrierTypeIds = {CTypeIdFunctionRtoR};
                    constraints.RequiredTags = {"plot", "artist", "function"};

                    FImplementationCostHintsV1 costHints;
                    costHints.ComplexityClass = "O(1)";
                    costHints.LatencyClass = "interactive";

                    (void) overlay.AttachOperationAsImplementationOf(
                        interfaceSchema.InterfaceId,
                        operationSchema.OperationId,
                        COperatorIdIdentityV1,
                        constraints,
                        costHints,
                        &semanticCatalog);

                    (void) overlay.AttachOperationPortSpaces(
                        interfaceSchema.InterfaceId,
                        operationSchema.OperationId,
                        {{"value", CSpaceIdFunctionRtoRV1}},
                        {});
                }

                if (operationSchema.OperationId == "command.artist.double_sample_count") {
                    FImplementationBindingConstraintsV1 constraints;
                    constraints.RequiredTags = {"plot", "artist", "resample"};

                    FImplementationCostHintsV1 costHints;
                    costHints.ComplexityClass = "O(n)";
                    costHints.LatencyClass = "interactive";

                    (void) overlay.AttachOperationAsImplementationOf(
                        interfaceSchema.InterfaceId,
                        operationSchema.OperationId,
                        COperatorIdResampleV1,
                        constraints,
                        costHints,
                        &semanticCatalog);
                }

                if (operationSchema.OperationId == "command.artist.set_domain") {
                    FImplementationBindingConstraintsV1 constraints;
                    constraints.RequiredCarrierTypeIds = {CTypeIdScalarString};
                    constraints.RequiredTags = {"plot", "artist", "domain"};

                    FImplementationCostHintsV1 costHints;
                    costHints.ComplexityClass = "O(1)";
                    costHints.LatencyClass = "interactive";

                    (void) overlay.AttachOperationAsImplementationOf(
                        interfaceSchema.InterfaceId,
                        operationSchema.OperationId,
                        COperatorIdResampleV1,
                        constraints,
                        costHints,
                        &semanticCatalog);
                }
            }
        }
    }

    inline auto GetReflectionSemanticsOverlayV1() -> FReflectionSemanticsOverlayV1 & {
        static FReflectionSemanticsOverlayV1 overlay;
        return overlay;
    }

    inline auto InitSemanticLayerV1() -> void {
        InitSemanticCatalogV1();
        (void) GetReflectionSemanticsOverlayV1();
    }

    inline auto SyncReflectionSemanticsOverlayV1(const FReflectionCatalogV2 &catalog,
                                                 const FAttachSemanticsOptionsV1 &options = {}) -> void {
        InitSemanticLayerV1();
        auto &overlay = GetReflectionSemanticsOverlayV1();
        auto &semanticCatalog = GetSemanticCatalogV1();
        overlay.Clear();
        AttachSemanticsForCatalogV1(catalog, overlay, semanticCatalog, options);
    }

    inline auto AttachParameterSpace(const Str &interfaceId, const Str &parameterId, const FSpaceId &spaceId) -> bool {
        InitSemanticLayerV1();
        return GetReflectionSemanticsOverlayV1().AttachParameterSpace(interfaceId, parameterId, spaceId);
    }

    inline auto AttachOperationPortSpaces(const Str &interfaceId,
                                          const Str &operationId,
                                          const Vector<FPortSpaceBindingV1> &inputSpaces,
                                          const Vector<FPortSpaceBindingV1> &outputSpaces) -> bool {
        InitSemanticLayerV1();
        return GetReflectionSemanticsOverlayV1().AttachOperationPortSpaces(
            interfaceId,
            operationId,
            inputSpaces,
            outputSpaces);
    }

    inline auto AttachOperationAsImplementationOf(const Str &interfaceId,
                                                  const Str &operationId,
                                                  const FSemanticOperatorId &semanticOperatorId,
                                                  const FImplementationBindingConstraintsV1 &constraints = {},
                                                  const FImplementationCostHintsV1 &costHints = {}) -> bool {
        InitSemanticLayerV1();
        return GetReflectionSemanticsOverlayV1().AttachOperationAsImplementationOf(
            interfaceId,
            operationId,
            semanticOperatorId,
            constraints,
            costHints,
            &GetSemanticCatalogV1());
    }

} // namespace Slab::Core::Reflection::V2

#endif // STUDIOSLAB_REFLECTION_SEMANTICS_OVERLAY_V1_H
