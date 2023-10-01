//
// Created by joao on 26/09/23.
//

#ifndef STUDIOSLAB_SNAPSHOTFILELOADER_H
#define STUDIOSLAB_SNAPSHOTFILELOADER_H

#include "Utils/String.h"
#include "Utils/PythonUtils.h"
#include "Maps/RtoR/Model/RtoRDiscreteFunctionCPU.h"

namespace Modes {

    class SnapshotFileLoader {
        std::shared_ptr<RtoR::DiscreteFunction_CPU> fieldPtr;

        static auto ReadPyDict(const Str& filePath) -> PythonUtils::PyDict;
        static auto ReadData(const Str& filePath) -> RealVector;

    public:
        SnapshotFileLoader(const Str &filename);

        static auto Load(const Str &filename) -> std::shared_ptr<RtoR::DiscreteFunction_CPU>;
    };

} // Modes

#endif //STUDIOSLAB_SNAPSHOTFILELOADER_H