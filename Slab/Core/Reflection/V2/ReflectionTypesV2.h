#ifndef STUDIOSLAB_REFLECTION_TYPES_V2_H
#define STUDIOSLAB_REFLECTION_TYPES_V2_H

#include "Utils/Types.h"

#include <algorithm>
#include <functional>
#include <map>
#include <optional>

namespace Slab::Core::Reflection::V2 {

    inline constexpr auto CCatalogVersionV2 = "0.1";

    enum class EParameterMutability : unsigned char {
        Const,
        RuntimeMutable,
        RestartRequired
    };

    enum class EParameterExposure : unsigned char {
        Hidden,
        ReadOnlyExposed,
        WritableExposed
    };

    enum class EOperationKind : unsigned char {
        Command,
        Query
    };

    enum class EThreadAffinity : unsigned char {
        Any,
        UI,
        Simulation,
        Worker
    };

    enum class ERunStatePolicy : unsigned char {
        Any,
        StoppedOnly,
        RunningOnly
    };

    enum class ESideEffectClass : unsigned char {
        None,
        LocalState,
        TaskLifecycle,
        IO,
        External
    };

    enum class EOperationStatus : unsigned char {
        Ok,
        Error
    };

    struct FReflectionValueV2 {
        Str TypeId;
        Str Encoded;

        FReflectionValueV2() = default;

        FReflectionValueV2(Str TypeId, Str Encoded)
        : TypeId(std::move(TypeId))
        , Encoded(std::move(Encoded)) {
        }
    };

    using FValueMapV2 = std::map<Str, FReflectionValueV2>;

    struct FParameterSchemaV2 {
        Str ParameterId;
        Str DisplayName;
        Str Description;
        Str TypeId;
        EParameterMutability Mutability = EParameterMutability::RuntimeMutable;
        EParameterExposure Exposure = EParameterExposure::WritableExposed;

        std::optional<FReflectionValueV2> DefaultValue;
        bool bSupportsCurrentValueProvider = true;
        bool bSupportsSetValueHandler = true;

        std::optional<DevFloat> Minimum;
        std::optional<DevFloat> Maximum;
        std::optional<DevFloat> Step;
        Str Unit;
        StrVector AllowedValues;

        Str UIControlType;
        Str UIGroup;
        std::optional<int> UIPrecision;

        bool bCodecCLI = true;
        bool bCodecJSON = true;
        bool bCodecScript = false;
    };

    struct FOperationFieldSchemaV2 {
        Str FieldId;
        Str DisplayName;
        Str Description;
        Str TypeId;
        bool bRequired = true;
    };

    struct FInvocationContextV2 {
        EThreadAffinity CurrentThread = EThreadAffinity::Any;
        bool bRuntimeRunning = false;
    };

    struct FOperationResultV2 {
        EOperationStatus Status = EOperationStatus::Ok;
        Str ErrorCode;
        Str ErrorMessage;

        FValueMapV2 OutputMap;

        std::optional<DevFloat> LatencyMs;
        Str ExecutedThread;
        StrVector Warnings;

        [[nodiscard]] auto IsOk() const -> bool { return Status == EOperationStatus::Ok; }

        static auto Ok(FValueMapV2 OutputMap = {}) -> FOperationResultV2 {
            FOperationResultV2 result;
            result.Status = EOperationStatus::Ok;
            result.OutputMap = std::move(OutputMap);
            return result;
        }

        static auto Error(Str Code, Str Message) -> FOperationResultV2 {
            FOperationResultV2 result;
            result.Status = EOperationStatus::Error;
            result.ErrorCode = std::move(Code);
            result.ErrorMessage = std::move(Message);
            return result;
        }
    };

    using FOperationHandlerV2 = std::function<FOperationResultV2(const FValueMapV2 &, const FInvocationContextV2 &)>;

    struct FOperationSchemaV2 {
        Str OperationId;
        Str DisplayName;
        Str Description;
        EOperationKind Kind = EOperationKind::Query;

        Vector<FOperationFieldSchemaV2> Inputs;
        Vector<FOperationFieldSchemaV2> Outputs;

        EThreadAffinity ThreadAffinity = EThreadAffinity::Any;
        ERunStatePolicy RunStatePolicy = ERunStatePolicy::Any;
        ESideEffectClass SideEffectClass = ESideEffectClass::None;

        FOperationHandlerV2 InvokeHandler;
    };

    struct FInterfaceSchemaV2 {
        Str InterfaceId;
        Str DisplayName;
        Str Description;
        int Version = 1;

        Vector<FParameterSchemaV2> Parameters;
        Vector<FOperationSchemaV2> Operations;
        Vector<Str> Children;
        Vector<Str> Tags;
    };

    struct FReflectionCatalogV2 {
        Str CatalogVersion = CCatalogVersionV2;
        Vector<FInterfaceSchemaV2> Interfaces;
    };

    inline auto ToString(const EParameterMutability value) -> const char * {
        switch (value) {
            case EParameterMutability::Const:
                return "Const";
            case EParameterMutability::RuntimeMutable:
                return "RuntimeMutable";
            case EParameterMutability::RestartRequired:
                return "RestartRequired";
        }

        return "Unknown";
    }

    inline auto ToString(const EParameterExposure value) -> const char * {
        switch (value) {
            case EParameterExposure::Hidden:
                return "Hidden";
            case EParameterExposure::ReadOnlyExposed:
                return "ReadOnlyExposed";
            case EParameterExposure::WritableExposed:
                return "WritableExposed";
        }

        return "Unknown";
    }

    inline auto ToString(const EOperationKind value) -> const char * {
        switch (value) {
            case EOperationKind::Command:
                return "Command";
            case EOperationKind::Query:
                return "Query";
        }

        return "Unknown";
    }

    inline auto ToString(const EThreadAffinity value) -> const char * {
        switch (value) {
            case EThreadAffinity::Any:
                return "Any";
            case EThreadAffinity::UI:
                return "UI";
            case EThreadAffinity::Simulation:
                return "Simulation";
            case EThreadAffinity::Worker:
                return "Worker";
        }

        return "Unknown";
    }

    inline auto ToString(const ERunStatePolicy value) -> const char * {
        switch (value) {
            case ERunStatePolicy::Any:
                return "Any";
            case ERunStatePolicy::StoppedOnly:
                return "StoppedOnly";
            case ERunStatePolicy::RunningOnly:
                return "RunningOnly";
        }

        return "Unknown";
    }

    inline auto ToString(const ESideEffectClass value) -> const char * {
        switch (value) {
            case ESideEffectClass::None:
                return "None";
            case ESideEffectClass::LocalState:
                return "LocalState";
            case ESideEffectClass::TaskLifecycle:
                return "TaskLifecycle";
            case ESideEffectClass::IO:
                return "IO";
            case ESideEffectClass::External:
                return "External";
        }

        return "Unknown";
    }

    inline auto ToString(const EOperationStatus value) -> const char * {
        switch (value) {
            case EOperationStatus::Ok:
                return "Ok";
            case EOperationStatus::Error:
                return "Error";
        }

        return "Unknown";
    }

    inline auto FindInterfaceById(FReflectionCatalogV2 &catalog, const Str &interfaceId) -> FInterfaceSchemaV2 * {
        const auto it = std::find_if(catalog.Interfaces.begin(), catalog.Interfaces.end(), [&](const FInterfaceSchemaV2 &schema) {
            return schema.InterfaceId == interfaceId;
        });

        if (it == catalog.Interfaces.end()) return nullptr;
        return &(*it);
    }

    inline auto FindInterfaceById(const FReflectionCatalogV2 &catalog, const Str &interfaceId) -> const FInterfaceSchemaV2 * {
        const auto it = std::find_if(catalog.Interfaces.begin(), catalog.Interfaces.end(), [&](const FInterfaceSchemaV2 &schema) {
            return schema.InterfaceId == interfaceId;
        });

        if (it == catalog.Interfaces.end()) return nullptr;
        return &(*it);
    }

    inline auto FindParameterById(FInterfaceSchemaV2 &interfaceSchema, const Str &parameterId) -> FParameterSchemaV2 * {
        const auto it = std::find_if(interfaceSchema.Parameters.begin(), interfaceSchema.Parameters.end(), [&](const FParameterSchemaV2 &schema) {
            return schema.ParameterId == parameterId;
        });

        if (it == interfaceSchema.Parameters.end()) return nullptr;
        return &(*it);
    }

    inline auto FindParameterById(const FInterfaceSchemaV2 &interfaceSchema, const Str &parameterId) -> const FParameterSchemaV2 * {
        const auto it = std::find_if(interfaceSchema.Parameters.begin(), interfaceSchema.Parameters.end(), [&](const FParameterSchemaV2 &schema) {
            return schema.ParameterId == parameterId;
        });

        if (it == interfaceSchema.Parameters.end()) return nullptr;
        return &(*it);
    }

    inline auto FindOperationById(FInterfaceSchemaV2 &interfaceSchema, const Str &operationId) -> FOperationSchemaV2 * {
        const auto it = std::find_if(interfaceSchema.Operations.begin(), interfaceSchema.Operations.end(), [&](const FOperationSchemaV2 &schema) {
            return schema.OperationId == operationId;
        });

        if (it == interfaceSchema.Operations.end()) return nullptr;
        return &(*it);
    }

    inline auto FindOperationById(const FInterfaceSchemaV2 &interfaceSchema, const Str &operationId) -> const FOperationSchemaV2 * {
        const auto it = std::find_if(interfaceSchema.Operations.begin(), interfaceSchema.Operations.end(), [&](const FOperationSchemaV2 &schema) {
            return schema.OperationId == operationId;
        });

        if (it == interfaceSchema.Operations.end()) return nullptr;
        return &(*it);
    }

} // namespace Slab::Core::Reflection::V2

#endif // STUDIOSLAB_REFLECTION_TYPES_V2_H
