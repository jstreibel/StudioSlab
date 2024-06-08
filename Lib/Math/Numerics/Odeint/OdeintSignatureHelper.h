//
// Created by joao on 6/1/24.
//

#ifndef STUDIOSLAB_ODEINTSIGNATUREHELPER_H
#define STUDIOSLAB_ODEINTSIGNATUREHELPER_H

#include <functional>

namespace Slab::Math {

    // Base template declaration (to handle cases not explicitly specialized)
    template <typename T, typename... Args>
    struct FunctionSignatureHelper;

// Helper template to generate the function signature
    template <typename T, typename... Args>
    struct FunctionSignatureHelper<T&, Args...> {
        using type = std::function<void(T&, Args...)>;
    };

// Generate a function signature with N `const T&` parameters
    template <int N, typename T, typename... Args>
    struct GenerateFunctionSignature {
        using type = typename GenerateFunctionSignature<N - 1, T, const T&, Args...>::type;
    };

    // Specialization for the base case (N = 0)
    template <typename T, typename... Args>
    struct GenerateFunctionSignature<0, T, Args...> {
        using type = typename FunctionSignatureHelper<T&, Args...>::type;
    };

    // Alias template for convenience
    template <int N, typename T>
    using OdeintOperation = typename GenerateFunctionSignature<N - 1, T>::type;









}

#endif //STUDIOSLAB_ODEINTSIGNATUREHELPER_H
