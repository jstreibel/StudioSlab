#include "PlotReflectionCatalogV2.h"

#include <algorithm>

namespace Slab::Graphics::Plot2D::V2 {

    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    auto FPlotReflectionCatalogV2::CapturePendingValues() const -> std::map<Str, FReflectionValueV2> {
        std::map<Str, FReflectionValueV2> pending;

        for (const auto &[interfaceId, interfaceBinding] : InterfacesById) {
            for (const auto &[parameterId, parameterBinding] : interfaceBinding.ParametersById) {
                if (!parameterBinding.PendingRestartValue.has_value()) continue;
                pending[MakePendingKey(interfaceId, parameterId)] = parameterBinding.PendingRestartValue.value();
            }
        }

        return pending;
    }

    auto FPlotReflectionCatalogV2::MakePendingKey(const Str &interfaceId, const Str &parameterId) -> Str {
        return interfaceId + "::" + parameterId;
    }

    auto FPlotReflectionCatalogV2::RebuildCatalogVector() -> void {
        Catalog.Interfaces.clear();
        Catalog.Interfaces.reserve(InterfacesById.size());

        for (auto &[interfaceId, boundInterface] : InterfacesById) {
            (void) interfaceId;
            Catalog.Interfaces.push_back(boundInterface.Schema);
        }

        std::sort(Catalog.Interfaces.begin(), Catalog.Interfaces.end(), [](const auto &lhs, const auto &rhs) {
            return lhs.InterfaceId < rhs.InterfaceId;
        });
    }

    auto FPlotReflectionCatalogV2::AddEntityDescriptor(
        FPlotEntityReflectionDescriptorV2 descriptor,
        const std::map<Str, FReflectionValueV2> &previousPending) -> void {
        if (descriptor.InterfaceId.empty()) return;

        FBoundInterface boundInterface;
        boundInterface.Schema.InterfaceId = std::move(descriptor.InterfaceId);
        boundInterface.Schema.DisplayName = std::move(descriptor.DisplayName);
        boundInterface.Schema.Description = std::move(descriptor.Description);
        boundInterface.Schema.Version = descriptor.Version;
        boundInterface.Schema.Children = std::move(descriptor.Children);
        boundInterface.Schema.Tags = std::move(descriptor.Tags);

        boundInterface.Schema.Parameters.reserve(descriptor.Parameters.size());

        for (auto &parameterBinding : descriptor.Parameters) {
            if (parameterBinding.Schema.ParameterId.empty()) continue;

            const auto key = MakePendingKey(boundInterface.Schema.InterfaceId, parameterBinding.Schema.ParameterId);

            FBoundParameter boundParameter;
            boundParameter.Schema = parameterBinding.Schema;
            boundParameter.ReadCurrent = parameterBinding.ReadCurrent;
            boundParameter.WriteLiveValue = parameterBinding.WriteLiveValue;

            if (const auto it = previousPending.find(key); it != previousPending.end()) {
                boundParameter.PendingRestartValue = it->second;
            }

            boundInterface.ParametersById[boundParameter.Schema.ParameterId] = std::move(boundParameter);
            boundInterface.Schema.Parameters.push_back(parameterBinding.Schema);
        }

        boundInterface.Schema.Operations = BuildDefaultOperations(boundInterface.Schema.InterfaceId);
        for (auto &op : descriptor.Operations) {
            boundInterface.Schema.Operations.push_back(std::move(op));
        }

        InterfacesById[boundInterface.Schema.InterfaceId] = std::move(boundInterface);
    }

    auto FPlotReflectionCatalogV2::BuildDefaultOperations(const Str &interfaceId) -> Vector<FOperationSchemaV2> {
        Vector<FOperationSchemaV2> operations;
        operations.reserve(4);

        FOperationSchemaV2 listParameters;
        listParameters.OperationId = CPlotOperationIdQueryListParametersV2;
        listParameters.DisplayName = "List Parameters";
        listParameters.Description = "Query current parameter values for this interface.";
        listParameters.Kind = ReflectionV2::EOperationKind::Query;
        listParameters.ThreadAffinity = ReflectionV2::EThreadAffinity::Any;
        listParameters.RunStatePolicy = ReflectionV2::ERunStatePolicy::Any;
        listParameters.SideEffectClass = ReflectionV2::ESideEffectClass::None;
        listParameters.InvokeHandler = [this, interfaceId](const FValueMapV2 &inputs, const FInvocationContextV2 &) {
            return HandleQueryListParameters(interfaceId, inputs);
        };
        operations.push_back(std::move(listParameters));

        FOperationSchemaV2 getParameter;
        getParameter.OperationId = CPlotOperationIdQueryGetParameterV2;
        getParameter.DisplayName = "Get Parameter";
        getParameter.Description = "Query one parameter by parameter_id.";
        getParameter.Kind = ReflectionV2::EOperationKind::Query;
        getParameter.ThreadAffinity = ReflectionV2::EThreadAffinity::Any;
        getParameter.RunStatePolicy = ReflectionV2::ERunStatePolicy::Any;
        getParameter.SideEffectClass = ReflectionV2::ESideEffectClass::None;
        getParameter.Inputs = {
            ReflectionV2::FOperationFieldSchemaV2{
                "parameter_id",
                "Parameter Id",
                "Reflection parameter id.",
                ReflectionV2::CTypeIdScalarString,
                true
            }
        };
        getParameter.InvokeHandler = [this, interfaceId](const FValueMapV2 &inputs, const FInvocationContextV2 &) {
            return HandleQueryGetParameter(interfaceId, inputs);
        };
        operations.push_back(std::move(getParameter));

        FOperationSchemaV2 setParameter;
        setParameter.OperationId = CPlotOperationIdCommandSetParameterV2;
        setParameter.DisplayName = "Set Parameter";
        setParameter.Description = "Set one parameter by parameter_id.";
        setParameter.Kind = ReflectionV2::EOperationKind::Command;
        setParameter.ThreadAffinity = ReflectionV2::EThreadAffinity::Any;
        setParameter.RunStatePolicy = ReflectionV2::ERunStatePolicy::Any;
        setParameter.SideEffectClass = ReflectionV2::ESideEffectClass::LocalState;
        setParameter.Inputs = {
            ReflectionV2::FOperationFieldSchemaV2{
                "parameter_id",
                "Parameter Id",
                "Reflection parameter id.",
                ReflectionV2::CTypeIdScalarString,
                true
            },
            ReflectionV2::FOperationFieldSchemaV2{
                "value",
                "Value",
                "Encoded parameter value.",
                ReflectionV2::CTypeIdScalarString,
                true
            }
        };
        setParameter.InvokeHandler = [this, interfaceId](const FValueMapV2 &inputs, const FInvocationContextV2 &) {
            return HandleCommandSetParameter(interfaceId, inputs);
        };
        operations.push_back(std::move(setParameter));

        FOperationSchemaV2 applyPending;
        applyPending.OperationId = CPlotOperationIdCommandApplyPendingV2;
        applyPending.DisplayName = "Apply Pending";
        applyPending.Description = "Apply RestartRequired staged parameter values.";
        applyPending.Kind = ReflectionV2::EOperationKind::Command;
        applyPending.ThreadAffinity = ReflectionV2::EThreadAffinity::Any;
        applyPending.RunStatePolicy = ReflectionV2::ERunStatePolicy::Any;
        applyPending.SideEffectClass = ReflectionV2::ESideEffectClass::LocalState;
        applyPending.InvokeHandler = [this, interfaceId](const FValueMapV2 &inputs, const FInvocationContextV2 &) {
            return HandleCommandApplyPending(interfaceId, inputs);
        };
        operations.push_back(std::move(applyPending));

        return operations;
    }

    auto FPlotReflectionCatalogV2::HandleQueryListParameters(const Str &interfaceId,
                                                             const FValueMapV2 &inputs) -> FOperationResultV2 {
        (void) inputs;

        auto *interfaceBinding = FindInterfaceBinding(interfaceId);
        if (interfaceBinding == nullptr) {
            return ReflectionV2::FOperationResultV2::Error(
                "reflection.interface.not_found",
                "Interface '" + interfaceId + "' not found.");
        }

        FValueMapV2 output;
        output["interface_id"] = ReflectionV2::MakeStringValue(interfaceId);
        output["parameter_count"] = ReflectionV2::MakeIntValue(static_cast<int>(interfaceBinding->ParametersById.size()));

        for (const auto &[parameterId, parameterBinding] : interfaceBinding->ParametersById) {
            if (!parameterBinding.ReadCurrent) continue;

            output["value." + parameterId] = parameterBinding.ReadCurrent();
            if (parameterBinding.PendingRestartValue.has_value()) {
                output["pending." + parameterId] = parameterBinding.PendingRestartValue.value();
            }
        }

        return ReflectionV2::FOperationResultV2::Ok(std::move(output));
    }

    auto FPlotReflectionCatalogV2::HandleQueryGetParameter(const Str &interfaceId,
                                                            const FValueMapV2 &inputs) -> FOperationResultV2 {
        const auto parameterId = ReadStringInput(inputs, "parameter_id");
        if (!parameterId.has_value()) {
            return ReflectionV2::FOperationResultV2::Error(
                "reflection.operation.missing_input",
                "Missing required input 'parameter_id'.");
        }

        auto *parameterBinding = FindParameterBinding(interfaceId, parameterId.value());
        if (parameterBinding == nullptr) {
            return ReflectionV2::FOperationResultV2::Error(
                "reflection.parameter.not_found",
                "Parameter '" + parameterId.value() + "' not found in interface '" + interfaceId + "'.");
        }

        FValueMapV2 output;
        output["interface_id"] = ReflectionV2::MakeStringValue(interfaceId);
        output["parameter_id"] = ReflectionV2::MakeStringValue(parameterBinding->Schema.ParameterId);
        output["parameter_type"] = ReflectionV2::MakeStringValue(parameterBinding->Schema.TypeId);
        output["mutability"] = ReflectionV2::MakeStringValue(ReflectionV2::ToString(parameterBinding->Schema.Mutability));

        if (parameterBinding->ReadCurrent) {
            output["value"] = parameterBinding->ReadCurrent();
        }

        if (parameterBinding->PendingRestartValue.has_value()) {
            output["pending_value"] = parameterBinding->PendingRestartValue.value();
        }

        return ReflectionV2::FOperationResultV2::Ok(std::move(output));
    }

    auto FPlotReflectionCatalogV2::HandleCommandSetParameter(const Str &interfaceId,
                                                              const FValueMapV2 &inputs) -> FOperationResultV2 {
        const auto parameterId = ReadStringInput(inputs, "parameter_id");
        if (!parameterId.has_value()) {
            return ReflectionV2::FOperationResultV2::Error(
                "reflection.operation.missing_input",
                "Missing required input 'parameter_id'.");
        }

        const auto value = ReadValueInput(inputs, "value");
        if (!value.has_value()) {
            return ReflectionV2::FOperationResultV2::Error(
                "reflection.operation.missing_input",
                "Missing required input 'value'.");
        }

        auto *parameterBinding = FindParameterBinding(interfaceId, parameterId.value());
        if (parameterBinding == nullptr) {
            return ReflectionV2::FOperationResultV2::Error(
                "reflection.parameter.not_found",
                "Parameter '" + parameterId.value() + "' not found in interface '" + interfaceId + "'.");
        }

        if (parameterBinding->Schema.Mutability == ReflectionV2::EParameterMutability::Const) {
            return ReflectionV2::FOperationResultV2::Error(
                "reflection.parameter.const",
                "Parameter '" + parameterId.value() + "' is const and cannot be set.");
        }

        FReflectionValueV2 typedValue = value.value();
        if (typedValue.TypeId.empty() || typedValue.TypeId == ReflectionV2::CTypeIdScalarString) {
            typedValue.TypeId = parameterBinding->Schema.TypeId;
        }

        if (typedValue.TypeId != parameterBinding->Schema.TypeId) {
            return ReflectionV2::FOperationResultV2::Error(
                "reflection.parameter.type_mismatch",
                "Parameter '" + parameterId.value() + "' expects type '"
                    + parameterBinding->Schema.TypeId + "' but got '" + typedValue.TypeId + "'.");
        }

        FValueMapV2 output;
        output["interface_id"] = ReflectionV2::MakeStringValue(interfaceId);
        output["parameter_id"] = ReflectionV2::MakeStringValue(parameterId.value());

        if (parameterBinding->Schema.Mutability == ReflectionV2::EParameterMutability::RestartRequired) {
            parameterBinding->PendingRestartValue = typedValue;
            output["staged"] = ReflectionV2::MakeBoolValue(true);
            output["value"] = typedValue;
            return ReflectionV2::FOperationResultV2::Ok(std::move(output));
        }

        if (!parameterBinding->WriteLiveValue) {
            return ReflectionV2::FOperationResultV2::Error(
                "reflection.parameter.no_set_handler",
                "Parameter '" + parameterId.value() + "' has no live set handler.");
        }

        auto applyResult = parameterBinding->WriteLiveValue(typedValue);
        if (!applyResult.IsOk()) return applyResult;

        parameterBinding->PendingRestartValue.reset();
        output["staged"] = ReflectionV2::MakeBoolValue(false);
        output["value"] = typedValue;
        return ReflectionV2::FOperationResultV2::Ok(std::move(output));
    }

    auto FPlotReflectionCatalogV2::HandleCommandApplyPending(const Str &interfaceId,
                                                              const FValueMapV2 &inputs) -> FOperationResultV2 {
        (void) inputs;

        auto *interfaceBinding = FindInterfaceBinding(interfaceId);
        if (interfaceBinding == nullptr) {
            return ReflectionV2::FOperationResultV2::Error(
                "reflection.interface.not_found",
                "Interface '" + interfaceId + "' not found.");
        }

        int appliedCount = 0;

        for (auto &[parameterId, parameterBinding] : interfaceBinding->ParametersById) {
            if (!parameterBinding.PendingRestartValue.has_value()) continue;
            if (!parameterBinding.WriteLiveValue) {
                return ReflectionV2::FOperationResultV2::Error(
                    "reflection.parameter.no_set_handler",
                    "Parameter '" + parameterId + "' has staged value but no live set handler.");
            }

            auto applyResult = parameterBinding.WriteLiveValue(parameterBinding.PendingRestartValue.value());
            if (!applyResult.IsOk()) return applyResult;

            parameterBinding.PendingRestartValue.reset();
            ++appliedCount;
        }

        FValueMapV2 output;
        output["interface_id"] = ReflectionV2::MakeStringValue(interfaceId);
        output["applied_count"] = ReflectionV2::MakeIntValue(appliedCount);
        return ReflectionV2::FOperationResultV2::Ok(std::move(output));
    }

    auto FPlotReflectionCatalogV2::FindInterfaceBinding(const Str &interfaceId) -> FBoundInterface * {
        const auto it = InterfacesById.find(interfaceId);
        if (it == InterfacesById.end()) return nullptr;
        return &it->second;
    }

    auto FPlotReflectionCatalogV2::FindInterfaceBinding(const Str &interfaceId) const -> const FBoundInterface * {
        const auto it = InterfacesById.find(interfaceId);
        if (it == InterfacesById.end()) return nullptr;
        return &it->second;
    }

    auto FPlotReflectionCatalogV2::FindParameterBinding(const Str &interfaceId, const Str &parameterId) -> FBoundParameter * {
        auto *interfaceBinding = FindInterfaceBinding(interfaceId);
        if (interfaceBinding == nullptr) return nullptr;

        const auto it = interfaceBinding->ParametersById.find(parameterId);
        if (it == interfaceBinding->ParametersById.end()) return nullptr;
        return &it->second;
    }

    auto FPlotReflectionCatalogV2::FindParameterBinding(const Str &interfaceId,
                                                         const Str &parameterId) const -> const FBoundParameter * {
        const auto *interfaceBinding = FindInterfaceBinding(interfaceId);
        if (interfaceBinding == nullptr) return nullptr;

        const auto it = interfaceBinding->ParametersById.find(parameterId);
        if (it == interfaceBinding->ParametersById.end()) return nullptr;
        return &it->second;
    }

    auto FPlotReflectionCatalogV2::ReadStringInput(const FValueMapV2 &inputs, const Str &key) -> std::optional<Str> {
        if (const auto it = inputs.find(key); it != inputs.end()) {
            return it->second.Encoded;
        }

        return std::nullopt;
    }

    auto FPlotReflectionCatalogV2::ReadValueInput(const FValueMapV2 &inputs,
                                                   const Str &key) -> std::optional<FReflectionValueV2> {
        if (const auto it = inputs.find(key); it != inputs.end()) {
            return it->second;
        }

        return std::nullopt;
    }

    auto FPlotReflectionCatalogV2::RefreshFromLiveWindows() -> void {
        const auto previousPending = CapturePendingValues();

        InterfacesById.clear();
        Catalog = ReflectionV2::FReflectionCatalogV2{};

        auto windows = FPlot2DWindowV2::GetLiveWindows();
        std::sort(windows.begin(), windows.end(), [](const auto *lhs, const auto *rhs) {
            if (lhs == nullptr || rhs == nullptr) return lhs != nullptr;
            return lhs->GetWindowId() < rhs->GetWindowId();
        });

        for (auto *window : windows) {
            if (window == nullptr) continue;
            AddEntityDescriptor(window->DescribeReflection(), previousPending);

            for (const auto &slot : window->GetArtists()) {
                if (slot.Artist == nullptr) continue;
                AddEntityDescriptor(slot.Artist->DescribeReflection(), previousPending);
            }
        }

        RebuildCatalogVector();
    }

    auto FPlotReflectionCatalogV2::GetCatalog() const -> const FReflectionCatalogV2 & {
        return Catalog;
    }

    auto FPlotReflectionCatalogV2::GetInterface(const Str &interfaceId) const -> const FInterfaceSchemaV2 * {
        return ReflectionV2::FindInterfaceById(Catalog, interfaceId);
    }

    auto FPlotReflectionCatalogV2::GetParameter(const Str &interfaceId, const Str &parameterId) const
        -> const FParameterSchemaV2 * {
        const auto *interfaceSchema = GetInterface(interfaceId);
        if (interfaceSchema == nullptr) return nullptr;
        return ReflectionV2::FindParameterById(*interfaceSchema, parameterId);
    }

    auto FPlotReflectionCatalogV2::Invoke(const Str &interfaceId,
                                          const Str &operationId,
                                          const FValueMapV2 &inputs,
                                          const FInvocationContextV2 &context) -> FOperationResultV2 {
        const auto *interfaceSchema = ReflectionV2::FindInterfaceById(Catalog, interfaceId);
        if (interfaceSchema == nullptr) {
            return ReflectionV2::FOperationResultV2::Error(
                "reflection.interface.not_found",
                "Interface '" + interfaceId + "' not found.");
        }

        const auto *operation = ReflectionV2::FindOperationById(*interfaceSchema, operationId);
        if (operation == nullptr) {
            return ReflectionV2::FOperationResultV2::Error(
                "reflection.operation.not_found",
                "Operation '" + operationId + "' not found in interface '" + interfaceId + "'.");
        }

        return ReflectionV2::InvokeOperationV2(*operation, inputs, context);
    }

    auto FPlotReflectionCatalogV2::EncodeCurrentParameterValue(const Str &interfaceId,
                                                                const Str &parameterId) const
        -> std::optional<FReflectionValueV2> {
        const auto *binding = FindParameterBinding(interfaceId, parameterId);
        if (binding == nullptr || !binding->ReadCurrent) return std::nullopt;
        return binding->ReadCurrent();
    }

} // namespace Slab::Graphics::Plot2D::V2
