//
// Created by joao on 26/09/23.
//

#ifndef STUDIOSLAB_HISTORYFILELOADER_H
#define STUDIOSLAB_HISTORYFILELOADER_H

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Utils/PythonUtils.h"

namespace Modes {

    using namespace Slab;

    class HistoryFileLoader {
        enum DataType {fp32, fp64};

        static auto ReadPyDict(std::ifstream& file) -> PythonUtils::PyDict;
        static auto ReadData(std::ifstream &filePath, PythonUtils::PyDict pyDict) -> RealArray ;
    public:
        static auto Load(const Str &filename) -> Pointer<Math::R2toR::NumericFunction_CPU>;
    };

} // Modes

#endif //STUDIOSLAB_HISTORYFILELOADER_H
