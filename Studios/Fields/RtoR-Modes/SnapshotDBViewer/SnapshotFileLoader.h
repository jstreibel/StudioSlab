//
// Created by joao on 26/09/23.
//

#ifndef STUDIOSLAB_SNAPSHOTFILELOADER_H
#define STUDIOSLAB_SNAPSHOTFILELOADER_H

#include "Utils/String.h"
#include "Utils/PythonUtils.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"

namespace Modes {
    using namespace Slab;

    class SnapshotFileLoader {
        Pointer<Math::RtoR::NumericFunction_CPU> fieldPtr;

        static auto ReadPyDict(const Str& filePath) -> PythonUtils::PyDict;
        static auto ReadData(const Str& filePath) -> RealVector;

    public:
        SnapshotFileLoader(const Str &filename);

        static auto Load(const Str &filename) -> Pointer<Math::RtoR::NumericFunction_CPU>;
    };

} // Modes

#endif //STUDIOSLAB_SNAPSHOTFILELOADER_H
