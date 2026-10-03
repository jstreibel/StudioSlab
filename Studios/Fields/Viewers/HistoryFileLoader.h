//
// Created by joao on 26/09/23.
//

#ifndef STUDIOSLAB_HISTORYFILELOADER_H
#define STUDIOSLAB_HISTORYFILELOADER_H

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Utils/PythonUtils.h"

#include <fstream>

namespace Modes {

using namespace Slab;

struct FLoadedHistory {
    TPointer<Math::R2toR::NumericFunction_CPU> Phi;
    TPointer<Math::R2toR::NumericFunction_CPU> DPhiDt;
    FRealVector Timestamps;
    PythonUtils::PyDict MetaData;

    [[nodiscard]] auto HasStoredTimeDerivative() const -> bool {
        return DPhiDt != nullptr;
    }
};

class FHistoryFileLoader {
    enum EDataType { fp32, fp64 };

    struct FDecodedData {
        FRealVector Timestamps;
        FRealVector Phi;
        FRealVector DPhiDt;
        long N = 0;
        long M = 0;
        long Channels = 0;
    };

    static auto ReadPyDict(std::ifstream& file) -> PythonUtils::PyDict;
    static auto ReadData(std::ifstream& file, const PythonUtils::PyDict& pyDict) -> FDecodedData;

  public:
    static auto Load(const Str& filename) -> FLoadedHistory;
};

using HistoryFileLoader [[deprecated("Use FHistoryFileLoader")]] = FHistoryFileLoader;

} // namespace Modes

#endif // STUDIOSLAB_HISTORYFILELOADER_H
