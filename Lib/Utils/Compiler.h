//
// Created by joao on 8/24/24.
//

#ifndef STUDIOSLAB_COMPILER_H
#define STUDIOSLAB_COMPILER_H

#if defined(__clang__)
#define COMPILER_NAME "Clang"
#elif defined(__GNUC__) || defined(__GNUG__)
#define COMPILER_NAME "GCC"
#elif defined(_MSC_VER)
    #define COMPILER_NAME "MSVC"
#elif defined(__INTEL_COMPILER)
    #define COMPILER_NAME "Intel"
#elif defined(__EMSCRIPTEN__)
    #define COMPILER_NAME "Emscripten"
#elif defined(__MINGW32__) || defined(__MINGW64__)
    #define COMPILER_NAME "MinGW"
#elif defined(__PGI)
    #define COMPILER_NAME "PGI"
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
    #define COMPILER_NAME "Oracle Solaris Studio"
#elif defined(__ibmxl__) || defined(__xlC__)
    #define COMPILER_NAME "IBM XL"
#else
    #define COMPILER_NAME "Unknown Compiler"
#endif

#endif //STUDIOSLAB_COMPILER_H
