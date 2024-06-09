//
// Created by joao on 26/09/23.
//

#ifndef STUDIOSLAB_HISTORYFILELOADER_H
#define STUDIOSLAB_HISTORYFILELOADER_H

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Utils/PythonUtils.h"

namespace Modes {

    class HistoryFileLoader {
        enum DataType {fp32, fp64};

        static auto ReadPyDict(std::ifstream& file) -> PythonUtils::PyDict;
        static auto ReadData(std::ifstream &filePath, PythonUtils::PyDict pyDict) -> RealArray ;
    public:
        static std::unique_ptr<R2toR::NumericFunction_CPU> Load(const Str &filename);
    };

} // Modes

#endif //STUDIOSLAB_HISTORYFILELOADER_H
