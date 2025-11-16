//
// Result - fluent-style success/failure wrapper (inspired by C# FluentResults)
//

#ifndef STUDIOSLAB_RESULT_H
#define STUDIOSLAB_RESULT_H

#include <utility>
#include <type_traits>
#include <functional>
#include <stdexcept>
#include <optional>
#include "String.h"

namespace Slab {

    // Forward declaration for void specialization
    template<typename T>
    class TResult;

    // Generic Result<T>
    template<typename T>
    class TResult {
    public:
        // Factories
        static TResult Ok(T value) {
            TResult r; r.bSuccess = true; r.ValueStorage.emplace(std::move(value)); return r;
        }

        template<typename... Args>
        static TResult OkInPlace(Args&&... args) {
            TResult r; r.bSuccess = true; r.ValueStorage.emplace(std::forward<Args>(args)...); return r;
        }

        static TResult Fail(const Str &error) {
            TResult r; r.bSuccess = false; r.ErrorsStorage.push_back(error); return r;
        }

        static TResult Fail(const StrVector &errors) {
            TResult r; r.bSuccess = false; r.ErrorsStorage = errors; return r;
        }

        // Inspect
        [[nodiscard]] bool IsSuccess() const { return bSuccess; }
        [[nodiscard]] bool IsFailure() const { return !bSuccess; }

        // Accessors (throws on failure)
        const T &Value() const {
            if (IsFailure() || !ValueStorage.has_value()) { throw std::logic_error("Accessing value of failed Result"); }
            return *ValueStorage;
        }

        T &Value() {
            if (IsFailure() || !ValueStorage.has_value()) { throw std::logic_error("Accessing value of failed Result"); }
            return *ValueStorage;
        }

        const StrVector &Errors() const { return ErrorsStorage; }
        const StrVector &Messages() const { return MessagesStorage; }

        // Concatenate errors and messages into a single string
        Str ToString() const {
            Str out;
            if (!ErrorsStorage.empty()) out += ToStr(ErrorsStorage);
            if (!MessagesStorage.empty()) out += ToStr(MessagesStorage);
            return out;
        }

        // Mutators (fluent)
        TResult &WithError(const Str &error) {
            bSuccess = false; ErrorsStorage.push_back(error); return *this;
        }

        TResult &WithErrors(StrVector errors) {
            bSuccess = false; for (auto &e : errors) ErrorsStorage.push_back(std::move(e)); return *this;
        }

        TResult &WithMessage(const Str &message) {
            MessagesStorage.push_back(message); return *this;
        }

        // Side-effect hooks
        template<typename F>
        const TResult &OnSuccess(F &&action) const {
            if (IsSuccess() && ValueStorage.has_value()) { std::invoke(std::forward<F>(action), *ValueStorage); }
            return *this;
        }

        template<typename F>
        const TResult &OnFailure(F &&action) const {
            if (IsFailure()) { std::invoke(std::forward<F>(action), ErrorsStorage); }
            return *this;
        }

        // Transform on success: T -> U (wraps into Result<U>)
        template<typename U, typename F>
        TResult<U> Map(F &&transform) const {
            if (IsFailure() || !ValueStorage.has_value()) { return TResult<U>::Fail(ErrorsStorage); }
            return TResult<U>::Ok(std::invoke(std::forward<F>(transform), *ValueStorage));
        }

        // Bind/Then on success: T -> Result<U>
        template<typename U, typename F>
        TResult<U> Then(F &&binder) const {
            if (IsFailure() || !ValueStorage.has_value()) { return TResult<U>::Fail(ErrorsStorage); }
            return std::invoke(std::forward<F>(binder), *ValueStorage);
        }

    private:
        bool bSuccess = false;
        std::optional<T> ValueStorage{};
        StrVector ErrorsStorage{};
        StrVector MessagesStorage{};
    };

    // Specialization for void
    template<>
    class TResult<void> {
    public:
        static TResult Ok() {
            TResult r; r.bSuccess = true; return r;
        }

        static TResult Fail(const Str &error) {
            TResult r; r.bSuccess = false; r.ErrorsStorage.push_back(error); return r;
        }

        static TResult Fail(const StrVector &errors) {
            TResult r; r.bSuccess = false; r.ErrorsStorage = errors; return r;
        }

        [[nodiscard]] bool IsSuccess() const { return bSuccess; }
        [[nodiscard]] bool IsFailure() const { return !bSuccess; }

        const StrVector &Errors() const { return ErrorsStorage; }
        const StrVector &Messages() const { return MessagesStorage; }

        // Concatenate errors and messages into a single string
        Str ToString() const {
            Str out;
            if (!ErrorsStorage.empty()) out += ToStr(ErrorsStorage);
            if (!MessagesStorage.empty()) out += ToStr(MessagesStorage);
            return out;
        }

        TResult &WithError(const Str &error) {
            bSuccess = false; ErrorsStorage.push_back(error); return *this;
        }

        TResult &WithErrors(StrVector errors) {
            bSuccess = false; for (auto &e : errors) ErrorsStorage.push_back(std::move(e)); return *this;
        }

        TResult &WithMessage(const Str &message) {
            MessagesStorage.push_back(message); return *this;
        }

        template<typename F>
        const TResult &OnSuccess(F &&action) const {
            if (IsSuccess()) { std::invoke(std::forward<F>(action)); }
            return *this;
        }

        template<typename F>
        const TResult &OnFailure(F &&action) const {
            if (IsFailure()) { std::invoke(std::forward<F>(action), ErrorsStorage); }
            return *this;
        }

        // Map/Then for void: forward to function producing Result<U>
        template<typename U, typename F>
        TResult<U> Then(F &&binder) const {
            if (IsFailure()) { return TResult<U>::Fail(ErrorsStorage); }
            return std::invoke(std::forward<F>(binder));
        }

    private:
        bool bSuccess = false;
        StrVector ErrorsStorage{};
        StrVector MessagesStorage{};
    };

} // namespace Slab

#endif // STUDIOSLAB_RESULT_H
