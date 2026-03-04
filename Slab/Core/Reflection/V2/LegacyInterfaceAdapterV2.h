#ifndef STUDIOSLAB_LEGACY_INTERFACE_ADAPTER_V2_H
#define STUDIOSLAB_LEGACY_INTERFACE_ADAPTER_V2_H

#include "ReflectionCodecsV2.h"
#include "ReflectionInvokeV2.h"

#include "Core/Controller/InterfaceManager.h"
#include "Core/Controller/Parameter/BuiltinParameters.h"

#include <algorithm>
#include <cctype>
#include <set>

namespace Slab::Core::Reflection::V2 {

    inline constexpr auto COperationIdQueryListParameters = "query.parameters";
    inline constexpr auto COperationIdQueryGetParameter = "query.parameter.get";
    inline constexpr auto COperationIdCommandSetParameter = "command.parameter.set";
    inline constexpr auto COperationIdCommandApplyPending = "command.parameter.apply_pending";

    class FLegacyReflectionCatalogAdapterV2 {
    public:
        auto RefreshFromLegacyInterfaces() -> void {
            const auto previousPending = CapturePendingValues();

            InterfacesById.clear();
            Catalog = FReflectionCatalogV2{};

            auto legacyInterfaces = Core::FInterfaceManager::GetInstance().GetInterfaces();
            Vector<TPointer<Core::FInterface>> mutableInterfaces;
            mutableInterfaces.reserve(legacyInterfaces.size());
            for (const auto &legacyConst : legacyInterfaces) {
                if (legacyConst == nullptr) continue;
                mutableInterfaces.emplace_back(ConstPointerCast<Core::FInterface>(legacyConst));
            }

            std::sort(mutableInterfaces.begin(), mutableInterfaces.end(), [](const auto &lhs, const auto &rhs) {
                if (lhs == nullptr || rhs == nullptr) return lhs != nullptr;
                return lhs->GetName() < rhs->GetName();
            });

            std::set<Str> usedInterfaceIds;
            for (const auto &legacyInterface : mutableInterfaces) {
                if (legacyInterface == nullptr) continue;
                AddLegacyInterface(*legacyInterface, usedInterfaceIds, previousPending);
            }

            AddFunctionSandboxInterface(previousPending);
            RebuildCatalogVector();
        }

        [[nodiscard]] auto GetCatalog() const -> const FReflectionCatalogV2 & {
            return Catalog;
        }

        [[nodiscard]] auto GetInterface(const Str &interfaceId) const -> const FInterfaceSchemaV2 * {
            return FindInterfaceById(Catalog, interfaceId);
        }

        [[nodiscard]] auto GetParameter(const Str &interfaceId, const Str &parameterId) const -> const FParameterSchemaV2 * {
            const auto *interfaceSchema = GetInterface(interfaceId);
            if (interfaceSchema == nullptr) return nullptr;
            return FindParameterById(*interfaceSchema, parameterId);
        }

        auto Invoke(const Str &interfaceId,
                    const Str &operationId,
                    const FValueMapV2 &inputs,
                    const FInvocationContextV2 &context) -> FOperationResultV2 {
            const auto *interfaceSchema = FindInterfaceById(Catalog, interfaceId);
            if (interfaceSchema == nullptr) {
                return FOperationResultV2::Error(
                    "reflection.interface.not_found",
                    "Interface '" + interfaceId + "' not found.");
            }

            const auto *operation = FindOperationById(*interfaceSchema, operationId);
            if (operation == nullptr) {
                return FOperationResultV2::Error(
                    "reflection.operation.not_found",
                    "Operation '" + operationId + "' not found in interface '" + interfaceId + "'.");
            }

            return InvokeOperationV2(*operation, inputs, context);
        }

        [[nodiscard]] auto EncodeCurrentParameterValue(const Str &interfaceId, const Str &parameterId) const
            -> std::optional<FReflectionValueV2> {
            const auto *binding = FindParameterBinding(interfaceId, parameterId);
            if (binding == nullptr || !binding->ReadCurrent) return std::nullopt;
            return binding->ReadCurrent();
        }

    private:
        struct FBoundParameter {
            FParameterSchemaV2 Schema;
            std::optional<FReflectionValueV2> PendingRestartValue;
            std::function<FReflectionValueV2()> ReadCurrent;
            std::function<FOperationResultV2(const FReflectionValueV2 &)> WriteLiveValue;
        };

        struct FBoundInterface {
            FInterfaceSchemaV2 Schema;
            std::map<Str, FBoundParameter> ParametersById;
        };

        FReflectionCatalogV2 Catalog;
        std::map<Str, FBoundInterface> InterfacesById;

        FFunctionDescriptorV2 FunctionSandboxLiveValue;

        [[nodiscard]] auto CapturePendingValues() const -> std::map<Str, FReflectionValueV2> {
            std::map<Str, FReflectionValueV2> pending;
            for (const auto &[interfaceId, interfaceBinding] : InterfacesById) {
                for (const auto &[parameterId, parameterBinding] : interfaceBinding.ParametersById) {
                    if (!parameterBinding.PendingRestartValue.has_value()) continue;
                    pending[MakePendingKey(interfaceId, parameterId)] = parameterBinding.PendingRestartValue.value();
                }
            }

            return pending;
        }

        static auto MakePendingKey(const Str &interfaceId, const Str &parameterId) -> Str {
            return interfaceId + "::" + parameterId;
        }

        static auto NormalizeToken(Str raw) -> Str {
            for (char &ch : raw) {
                if (std::isalnum(static_cast<unsigned char>(ch))) {
                    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
                } else {
                    ch = '_';
                }
            }

            Str out;
            out.reserve(raw.size());
            bool lastUnderscore = false;
            for (const char ch : raw) {
                const bool isUnderscore = ch == '_';
                if (isUnderscore) {
                    if (lastUnderscore) continue;
                    lastUnderscore = true;
                } else {
                    lastUnderscore = false;
                }
                out.push_back(ch);
            }

            while (!out.empty() && out.front() == '_') out.erase(out.begin());
            while (!out.empty() && out.back() == '_') out.pop_back();
            if (out.empty()) out = "item";
            return out;
        }

        static auto NormalizeTokenKebab(Str raw) -> Str {
            for (char &ch : raw) {
                if (std::isalnum(static_cast<unsigned char>(ch))) {
                    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
                } else {
                    ch = '-';
                }
            }

            Str out;
            out.reserve(raw.size());
            bool lastDash = false;
            for (const char ch : raw) {
                const bool isDash = ch == '-';
                if (isDash) {
                    if (lastDash) continue;
                    lastDash = true;
                } else {
                    lastDash = false;
                }
                out.push_back(ch);
            }

            while (!out.empty() && out.front() == '-') out.erase(out.begin());
            while (!out.empty() && out.back() == '-') out.pop_back();
            if (out.empty()) out = "item";
            return out;
        }

        static auto MakeUniqueId(const Str &base, std::set<Str> &used) -> Str {
            if (!used.contains(base)) {
                used.insert(base);
                return base;
            }

            int suffix = 2;
            while (true) {
                const auto candidate = base + "_" + ToStr(suffix);
                if (!used.contains(candidate)) {
                    used.insert(candidate);
                    return candidate;
                }
                ++suffix;
            }
        }

        auto AddLegacyInterface(const Core::FInterface &legacyInterface,
                                std::set<Str> &usedInterfaceIds,
                                const std::map<Str, FReflectionValueV2> &previousPending) -> void {
            Str interfaceId = "legacy." + NormalizeToken(legacyInterface.GetName());
            interfaceId = MakeUniqueId(interfaceId, usedInterfaceIds);

            FBoundInterface boundInterface;
            boundInterface.Schema.InterfaceId = interfaceId;
            boundInterface.Schema.DisplayName = legacyInterface.GetName();
            boundInterface.Schema.Description = legacyInterface.GetGeneralDescription();
            boundInterface.Schema.Version = 1;
            boundInterface.Schema.Tags = {"legacy", "adapter"};

            std::set<Str> usedParameterIds;
            for (const auto &parameterConst : legacyInterface.GetParameters()) {
                if (parameterConst == nullptr) continue;
                auto parameter = ConstPointerCast<Core::FParameter>(parameterConst);

                FBoundParameter boundParameter;
                boundParameter.Schema = BuildParameterSchema(*parameter, usedParameterIds);

                const auto pendingKey = MakePendingKey(interfaceId, boundParameter.Schema.ParameterId);
                if (const auto it = previousPending.find(pendingKey); it != previousPending.end()) {
                    boundParameter.PendingRestartValue = it->second;
                }

                const auto typeId = boundParameter.Schema.TypeId;
                boundParameter.ReadCurrent = [parameter, typeId]() {
                    return FReflectionValueV2(typeId, parameter->ValueToString());
                };

                boundParameter.WriteLiveValue = [parameter](const FReflectionValueV2 &value) {
                    return WriteLegacyParameter(*parameter, value.Encoded);
                };

                boundInterface.Schema.Parameters.push_back(boundParameter.Schema);
                boundInterface.ParametersById[boundParameter.Schema.ParameterId] = boundParameter;
            }

            for (const auto &child : legacyInterface.GetSubInterfaces()) {
                if (child == nullptr) continue;
                boundInterface.Schema.Children.push_back("legacy." + NormalizeToken(child->GetName()));
            }

            boundInterface.Schema.Operations = BuildDefaultOperations(interfaceId);

            InterfacesById[interfaceId] = boundInterface;
        }

        auto AddFunctionSandboxInterface(const std::map<Str, FReflectionValueV2> &previousPending) -> void {
            const auto SandboxInterfaceId = "v2." + NormalizeTokenKebab("function sandbox");
            const auto SandboxParameterId = NormalizeTokenKebab("initial condition");

            FBoundInterface sandbox;
            sandbox.Schema.InterfaceId = SandboxInterfaceId;
            sandbox.Schema.DisplayName = "Function Sandbox";
            sandbox.Schema.Description = "Reflection V2 complex-parameter sandbox (r_to_r function descriptor).";
            sandbox.Schema.Version = 1;
            sandbox.Schema.Tags = {"v2", "reflection", "complex-type", "demo"};

            FBoundParameter functionParameter;
            functionParameter.Schema.ParameterId = SandboxParameterId;
            functionParameter.Schema.DisplayName = "Initial Condition";
            functionParameter.Schema.Description = "Example descriptor for function-typed parameter round-trip.";
            functionParameter.Schema.TypeId = CTypeIdFunctionRtoR;
            functionParameter.Schema.Mutability = EParameterMutability::RestartRequired;
            functionParameter.Schema.Exposure = EParameterExposure::WritableExposed;
            functionParameter.Schema.UIControlType = "function_descriptor";
            functionParameter.Schema.bCodecCLI = true;
            functionParameter.Schema.bCodecJSON = true;
            functionParameter.Schema.bCodecScript = true;
            functionParameter.Schema.DefaultValue =
                FReflectionValueV2(CTypeIdFunctionRtoR, EncodeFunctionDescriptorCLI(FunctionSandboxLiveValue));

            const auto pendingKey = MakePendingKey(SandboxInterfaceId, SandboxParameterId);
            if (const auto it = previousPending.find(pendingKey); it != previousPending.end()) {
                functionParameter.PendingRestartValue = it->second;
            }

            functionParameter.ReadCurrent = [this]() {
                return FReflectionValueV2(CTypeIdFunctionRtoR, EncodeFunctionDescriptorCLI(FunctionSandboxLiveValue));
            };

            functionParameter.WriteLiveValue = [this](const FReflectionValueV2 &value) {
                FFunctionDescriptorV2 decoded;
                Str error;
                if (!DecodeFunctionDescriptorCLI(value.Encoded, decoded, &error)) {
                    return FOperationResultV2::Error(
                        "reflection.parameter.codec_error",
                        "Failed to decode function descriptor: " + error);
                }

                FunctionSandboxLiveValue = decoded;
                return FOperationResultV2::Ok();
            };

            sandbox.Schema.Parameters.push_back(functionParameter.Schema);
            sandbox.ParametersById[SandboxParameterId] = functionParameter;
            sandbox.Schema.Operations = BuildDefaultOperations(SandboxInterfaceId);

            InterfacesById[SandboxInterfaceId] = sandbox;
        }

        auto RebuildCatalogVector() -> void {
            Catalog.Interfaces.clear();
            Catalog.Interfaces.reserve(InterfacesById.size());

            for (const auto &[interfaceId, interfaceBinding] : InterfacesById) {
                (void) interfaceId;
                Catalog.Interfaces.push_back(interfaceBinding.Schema);
            }

            std::sort(Catalog.Interfaces.begin(), Catalog.Interfaces.end(), [](const auto &lhs, const auto &rhs) {
                if (lhs.DisplayName == rhs.DisplayName) return lhs.InterfaceId < rhs.InterfaceId;
                return lhs.DisplayName < rhs.DisplayName;
            });
        }

        auto BuildParameterSchema(const Core::FParameter &parameter, std::set<Str> &usedParameterIds) const
            -> FParameterSchemaV2 {
            FParameterSchemaV2 schema;

            Str key = parameter.GetCommandLineArgumentName(true);
            if (key.empty()) key = parameter.GetName();

            schema.ParameterId = MakeUniqueId(NormalizeToken(key), usedParameterIds);
            schema.DisplayName = parameter.GetName();
            schema.Description = parameter.GetDescription();
            schema.TypeId = InferTypeIdFromLegacyParameterType(parameter.GetType());
            schema.Mutability = EParameterMutability::RuntimeMutable;
            schema.Exposure = EParameterExposure::WritableExposed;
            schema.bSupportsCurrentValueProvider = true;
            schema.bSupportsSetValueHandler = true;

            if (parameter.GetType() == Core::EParameterType::ParameterType_Bool) {
                schema.UIControlType = "checkbox";
                schema.AllowedValues = {"true", "false"};
            }

            if (parameter.GetType() == Core::EParameterType::ParameterType_Integer) {
                const auto *integerParameter = dynamic_cast<const Core::IntegerParameter *>(&parameter);
                if (integerParameter != nullptr) {
                    const auto &attrs = integerParameter->GetAttributes().BuiltinAttributes;
                    if (const auto it = attrs.find(Core::CMinimumTag); it != attrs.end() && it->second.has_value()) {
                        schema.Minimum = static_cast<DevFloat>(it->second.value());
                    }
                    if (const auto it = attrs.find(Core::CMaximumTag); it != attrs.end() && it->second.has_value()) {
                        schema.Maximum = static_cast<DevFloat>(it->second.value());
                    }
                }
            }

            if (parameter.GetType() == Core::EParameterType::ParameterType_Float) {
                const auto *realParameter = dynamic_cast<const Core::RealParameter *>(&parameter);
                if (realParameter != nullptr) {
                    const auto &attrs = realParameter->GetAttributes().BuiltinAttributes;
                    if (const auto it = attrs.find(Core::CMinimumTag); it != attrs.end() && it->second.has_value()) {
                        schema.Minimum = it->second.value();
                    }
                    if (const auto it = attrs.find(Core::CMaximumTag); it != attrs.end() && it->second.has_value()) {
                        schema.Maximum = it->second.value();
                    }
                }
            }

            schema.DefaultValue = FReflectionValueV2(schema.TypeId, parameter.ValueToString());
            return schema;
        }

        static auto WriteLegacyParameter(Core::FParameter &parameter, const Str &encoded) -> FOperationResultV2 {
            try {
                switch (parameter.GetType()) {
                    case Core::EParameterType::ParameterType_Integer: {
                        const int parsed = std::stoi(encoded);
                        parameter.SetValue(&parsed);
                        return FOperationResultV2::Ok();
                    }
                    case Core::EParameterType::ParameterType_Float: {
                        const DevFloat parsed = std::stod(encoded);
                        parameter.SetValue(&parsed);
                        return FOperationResultV2::Ok();
                    }
                    case Core::EParameterType::ParameterType_String: {
                        const Str parsed = encoded;
                        parameter.SetValue(&parsed);
                        return FOperationResultV2::Ok();
                    }
                    case Core::EParameterType::ParameterType_MultiString: {
                        const auto parsed = ParseStringListValue(encoded);
                        parameter.SetValue(&parsed);
                        return FOperationResultV2::Ok();
                    }
                    case Core::EParameterType::ParameterType_Bool: {
                        bool parsed = false;
                        if (!ParseBoolValue(encoded, parsed)) {
                            return FOperationResultV2::Error(
                                "reflection.parameter.parse_error",
                                "Could not parse bool value '" + encoded + "'.");
                        }
                        parameter.SetValue(&parsed);
                        return FOperationResultV2::Ok();
                    }
                    case Core::EParameterType::ParameterType_Uninformed:
                        break;
                }
            } catch (const std::exception &e) {
                return FOperationResultV2::Error(
                    "reflection.parameter.parse_error",
                    "Failed to parse parameter value '" + encoded + "': " + e.what());
            }

            return FOperationResultV2::Error(
                "reflection.parameter.unsupported_type",
                "Unsupported legacy parameter type for parameter write.");
        }

        auto BuildDefaultOperations(const Str &interfaceId) -> Vector<FOperationSchemaV2> {
            Vector<FOperationSchemaV2> operations;
            operations.reserve(4);

            FOperationSchemaV2 listParameters;
            listParameters.OperationId = COperationIdQueryListParameters;
            listParameters.DisplayName = "List Parameters";
            listParameters.Description = "Query current parameter values for this interface.";
            listParameters.Kind = EOperationKind::Query;
            listParameters.ThreadAffinity = EThreadAffinity::Any;
            listParameters.RunStatePolicy = ERunStatePolicy::Any;
            listParameters.SideEffectClass = ESideEffectClass::None;
            listParameters.InvokeHandler = [this, interfaceId](const FValueMapV2 &inputs, const FInvocationContextV2 &) {
                return HandleQueryListParameters(interfaceId, inputs);
            };
            operations.push_back(listParameters);

            FOperationSchemaV2 getParameter;
            getParameter.OperationId = COperationIdQueryGetParameter;
            getParameter.DisplayName = "Get Parameter";
            getParameter.Description = "Query one parameter by parameter_id.";
            getParameter.Kind = EOperationKind::Query;
            getParameter.ThreadAffinity = EThreadAffinity::Any;
            getParameter.RunStatePolicy = ERunStatePolicy::Any;
            getParameter.SideEffectClass = ESideEffectClass::None;
            getParameter.Inputs = {
                FOperationFieldSchemaV2{"parameter_id", "Parameter Id", "Reflection parameter id.", CTypeIdScalarString, true}
            };
            getParameter.InvokeHandler = [this, interfaceId](const FValueMapV2 &inputs, const FInvocationContextV2 &) {
                return HandleQueryGetParameter(interfaceId, inputs);
            };
            operations.push_back(getParameter);

            FOperationSchemaV2 setParameter;
            setParameter.OperationId = COperationIdCommandSetParameter;
            setParameter.DisplayName = "Set Parameter";
            setParameter.Description = "Set one parameter by parameter_id.";
            setParameter.Kind = EOperationKind::Command;
            setParameter.ThreadAffinity = EThreadAffinity::Any;
            setParameter.RunStatePolicy = ERunStatePolicy::StoppedOnly;
            setParameter.SideEffectClass = ESideEffectClass::LocalState;
            setParameter.Inputs = {
                FOperationFieldSchemaV2{"parameter_id", "Parameter Id", "Reflection parameter id.", CTypeIdScalarString, true},
                FOperationFieldSchemaV2{"value", "Value", "Encoded parameter value.", CTypeIdScalarString, true}
            };
            setParameter.InvokeHandler = [this, interfaceId](const FValueMapV2 &inputs, const FInvocationContextV2 &) {
                return HandleCommandSetParameter(interfaceId, inputs);
            };
            operations.push_back(setParameter);

            FOperationSchemaV2 applyPending;
            applyPending.OperationId = COperationIdCommandApplyPending;
            applyPending.DisplayName = "Apply Pending";
            applyPending.Description = "Apply RestartRequired staged parameter values.";
            applyPending.Kind = EOperationKind::Command;
            applyPending.ThreadAffinity = EThreadAffinity::Any;
            applyPending.RunStatePolicy = ERunStatePolicy::StoppedOnly;
            applyPending.SideEffectClass = ESideEffectClass::TaskLifecycle;
            applyPending.InvokeHandler = [this, interfaceId](const FValueMapV2 &inputs, const FInvocationContextV2 &) {
                return HandleCommandApplyPending(interfaceId, inputs);
            };
            operations.push_back(applyPending);

            return operations;
        }

        auto HandleQueryListParameters(const Str &interfaceId, const FValueMapV2 &inputs) -> FOperationResultV2 {
            (void) inputs;

            auto *interfaceBinding = FindInterfaceBinding(interfaceId);
            if (interfaceBinding == nullptr) {
                return FOperationResultV2::Error(
                    "reflection.interface.not_found",
                    "Interface '" + interfaceId + "' not found.");
            }

            FValueMapV2 output;
            output["interface_id"] = MakeStringValue(interfaceId);
            output["parameter_count"] = MakeIntValue(static_cast<int>(interfaceBinding->ParametersById.size()));

            for (const auto &[parameterId, parameterBinding] : interfaceBinding->ParametersById) {
                if (!parameterBinding.ReadCurrent) continue;
                output["value." + parameterId] = parameterBinding.ReadCurrent();

                if (parameterBinding.PendingRestartValue.has_value()) {
                    output["pending." + parameterId] = parameterBinding.PendingRestartValue.value();
                }
            }

            return FOperationResultV2::Ok(std::move(output));
        }

        auto HandleQueryGetParameter(const Str &interfaceId, const FValueMapV2 &inputs) -> FOperationResultV2 {
            const auto parameterId = ReadStringInput(inputs, "parameter_id");
            if (!parameterId.has_value()) {
                return FOperationResultV2::Error(
                    "reflection.operation.missing_input",
                    "Missing required input 'parameter_id'.");
            }

            auto *parameterBinding = FindParameterBinding(interfaceId, parameterId.value());
            if (parameterBinding == nullptr) {
                return FOperationResultV2::Error(
                    "reflection.parameter.not_found",
                    "Parameter '" + parameterId.value() + "' not found in interface '" + interfaceId + "'.");
            }

            FValueMapV2 output;
            output["interface_id"] = MakeStringValue(interfaceId);
            output["parameter_id"] = MakeStringValue(parameterBinding->Schema.ParameterId);
            output["parameter_type"] = MakeStringValue(parameterBinding->Schema.TypeId);
            output["mutability"] = MakeStringValue(ToString(parameterBinding->Schema.Mutability));

            if (parameterBinding->ReadCurrent) {
                output["value"] = parameterBinding->ReadCurrent();
            }

            if (parameterBinding->PendingRestartValue.has_value()) {
                output["pending_value"] = parameterBinding->PendingRestartValue.value();
            }

            return FOperationResultV2::Ok(std::move(output));
        }

        auto HandleCommandSetParameter(const Str &interfaceId, const FValueMapV2 &inputs) -> FOperationResultV2 {
            const auto parameterId = ReadStringInput(inputs, "parameter_id");
            if (!parameterId.has_value()) {
                return FOperationResultV2::Error(
                    "reflection.operation.missing_input",
                    "Missing required input 'parameter_id'.");
            }

            const auto value = ReadValueInput(inputs, "value");
            if (!value.has_value()) {
                return FOperationResultV2::Error(
                    "reflection.operation.missing_input",
                    "Missing required input 'value'.");
            }

            auto *parameterBinding = FindParameterBinding(interfaceId, parameterId.value());
            if (parameterBinding == nullptr) {
                return FOperationResultV2::Error(
                    "reflection.parameter.not_found",
                    "Parameter '" + parameterId.value() + "' not found in interface '" + interfaceId + "'.");
            }

            if (parameterBinding->Schema.Mutability == EParameterMutability::Const) {
                return FOperationResultV2::Error(
                    "reflection.parameter.const",
                    "Parameter '" + parameterId.value() + "' is const and cannot be set.");
            }

            FReflectionValueV2 typedValue = value.value();
            if (typedValue.TypeId.empty() || typedValue.TypeId == CTypeIdScalarString) {
                typedValue.TypeId = parameterBinding->Schema.TypeId;
            }

            if (typedValue.TypeId != parameterBinding->Schema.TypeId) {
                return FOperationResultV2::Error(
                    "reflection.parameter.type_mismatch",
                    "Parameter '" + parameterId.value() + "' expects type '"
                        + parameterBinding->Schema.TypeId + "' but got '" + typedValue.TypeId + "'.");
            }

            FValueMapV2 output;
            output["interface_id"] = MakeStringValue(interfaceId);
            output["parameter_id"] = MakeStringValue(parameterId.value());

            if (parameterBinding->Schema.Mutability == EParameterMutability::RestartRequired) {
                parameterBinding->PendingRestartValue = typedValue;
                output["staged"] = MakeBoolValue(true);
                output["value"] = typedValue;
                return FOperationResultV2::Ok(std::move(output));
            }

            if (!parameterBinding->WriteLiveValue) {
                return FOperationResultV2::Error(
                    "reflection.parameter.no_set_handler",
                    "Parameter '" + parameterId.value() + "' has no live set handler.");
            }

            auto applyResult = parameterBinding->WriteLiveValue(typedValue);
            if (!applyResult.IsOk()) return applyResult;

            parameterBinding->PendingRestartValue.reset();
            output["staged"] = MakeBoolValue(false);
            output["value"] = typedValue;
            return FOperationResultV2::Ok(std::move(output));
        }

        auto HandleCommandApplyPending(const Str &interfaceId, const FValueMapV2 &inputs) -> FOperationResultV2 {
            (void) inputs;

            auto *interfaceBinding = FindInterfaceBinding(interfaceId);
            if (interfaceBinding == nullptr) {
                return FOperationResultV2::Error(
                    "reflection.interface.not_found",
                    "Interface '" + interfaceId + "' not found.");
            }

            int appliedCount = 0;
            for (auto &[parameterId, parameterBinding] : interfaceBinding->ParametersById) {
                if (!parameterBinding.PendingRestartValue.has_value()) continue;
                if (!parameterBinding.WriteLiveValue) {
                    return FOperationResultV2::Error(
                        "reflection.parameter.no_set_handler",
                        "Parameter '" + parameterId + "' has staged value but no live set handler.");
                }

                auto applyResult = parameterBinding.WriteLiveValue(parameterBinding.PendingRestartValue.value());
                if (!applyResult.IsOk()) return applyResult;

                parameterBinding.PendingRestartValue.reset();
                ++appliedCount;
            }

            FValueMapV2 output;
            output["interface_id"] = MakeStringValue(interfaceId);
            output["applied_count"] = MakeIntValue(appliedCount);
            return FOperationResultV2::Ok(std::move(output));
        }

        auto FindInterfaceBinding(const Str &interfaceId) -> FBoundInterface * {
            const auto it = InterfacesById.find(interfaceId);
            if (it == InterfacesById.end()) return nullptr;
            return &it->second;
        }

        auto FindInterfaceBinding(const Str &interfaceId) const -> const FBoundInterface * {
            const auto it = InterfacesById.find(interfaceId);
            if (it == InterfacesById.end()) return nullptr;
            return &it->second;
        }

        auto FindParameterBinding(const Str &interfaceId, const Str &parameterId) -> FBoundParameter * {
            auto *interfaceBinding = FindInterfaceBinding(interfaceId);
            if (interfaceBinding == nullptr) return nullptr;

            const auto it = interfaceBinding->ParametersById.find(parameterId);
            if (it == interfaceBinding->ParametersById.end()) return nullptr;
            return &it->second;
        }

        auto FindParameterBinding(const Str &interfaceId, const Str &parameterId) const -> const FBoundParameter * {
            const auto *interfaceBinding = FindInterfaceBinding(interfaceId);
            if (interfaceBinding == nullptr) return nullptr;

            const auto it = interfaceBinding->ParametersById.find(parameterId);
            if (it == interfaceBinding->ParametersById.end()) return nullptr;
            return &it->second;
        }

        static auto ReadStringInput(const FValueMapV2 &inputs, const Str &key) -> std::optional<Str> {
            if (const auto it = inputs.find(key); it != inputs.end()) {
                return it->second.Encoded;
            }

            return std::nullopt;
        }

        static auto ReadValueInput(const FValueMapV2 &inputs, const Str &key) -> std::optional<FReflectionValueV2> {
            if (const auto it = inputs.find(key); it != inputs.end()) {
                return it->second;
            }

            return std::nullopt;
        }
    };

} // namespace Slab::Core::Reflection::V2

#endif // STUDIOSLAB_LEGACY_INTERFACE_ADAPTER_V2_H
