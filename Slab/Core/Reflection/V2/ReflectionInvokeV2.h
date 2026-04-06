#ifndef STUDIOSLAB_REFLECTION_INVOKE_V2_H
#define STUDIOSLAB_REFLECTION_INVOKE_V2_H

#include "ReflectionTypesV2.h"

#include <chrono>

namespace Slab::Core::Reflection::V2 {

    inline auto BuildOperationError(const Str &code, const Str &message) -> FOperationResultV2 {
        return FOperationResultV2::Error(code, message);
    }

    inline auto ValidateInvocationContract(const FOperationSchemaV2 &operation,
                                           const FInvocationContextV2 &context,
                                           Str &outErrorCode,
                                           Str &outErrorMessage) -> bool {
        if (operation.Kind == EOperationKind::Query && operation.SideEffectClass != ESideEffectClass::None) {
            outErrorCode = "reflection.operation.invalid_query_side_effect";
            outErrorMessage = "Query operations must declare side effect class None.";
            return false;
        }

        if (operation.ThreadAffinity != EThreadAffinity::Any && operation.ThreadAffinity != context.CurrentThread) {
            outErrorCode = "reflection.operation.thread_affinity_violation";
            outErrorMessage = "Operation requires thread affinity '" + Str(ToString(operation.ThreadAffinity))
                + "' but invocation thread is '" + Str(ToString(context.CurrentThread)) + "'.";
            return false;
        }

        if (operation.RunStatePolicy == ERunStatePolicy::RunningOnly && !context.bRuntimeRunning) {
            outErrorCode = "reflection.operation.run_state_violation";
            outErrorMessage = "Operation requires runtime to be running.";
            return false;
        }

        if (operation.RunStatePolicy == ERunStatePolicy::StoppedOnly && context.bRuntimeRunning) {
            outErrorCode = "reflection.operation.run_state_violation";
            outErrorMessage = "Operation requires runtime to be stopped.";
            return false;
        }

        return true;
    }

    inline auto InvokeOperationV2(const FOperationSchemaV2 &operation,
                                  const FValueMapV2 &inputs,
                                  const FInvocationContextV2 &context) -> FOperationResultV2 {
        if (!operation.InvokeHandler) {
            return BuildOperationError(
                "reflection.operation.no_handler",
                "Operation '" + operation.OperationId + "' does not have an invoke handler.");
        }

        Str errorCode;
        Str errorMessage;
        if (!ValidateInvocationContract(operation, context, errorCode, errorMessage)) {
            return BuildOperationError(errorCode, errorMessage);
        }

        const auto begin = std::chrono::steady_clock::now();

        try {
            auto result = operation.InvokeHandler(inputs, context);
            if (result.ExecutedThread.empty()) {
                result.ExecutedThread = ToString(context.CurrentThread);
            }

            const auto end = std::chrono::steady_clock::now();
            const auto elapsed = std::chrono::duration_cast<std::chrono::duration<DevFloat, std::milli>>(end - begin);
            result.LatencyMs = elapsed.count();
            return result;
        } catch (const std::exception &e) {
            return BuildOperationError(
                "reflection.operation.exception",
                Str("Operation '") + operation.OperationId + "' threw exception: " + e.what());
        } catch (...) {
            return BuildOperationError(
                "reflection.operation.exception_unknown",
                Str("Operation '") + operation.OperationId + "' threw an unknown exception.");
        }
    }

} // namespace Slab::Core::Reflection::V2

#endif // STUDIOSLAB_REFLECTION_INVOKE_V2_H
