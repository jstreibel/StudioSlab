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

    namespace DatabaseViewer { class FDBParser; }

    struct FSnapshotData {
        TPointer<Math::RtoR::NumericFunction_CPU> data;
        PythonUtils::PyDict metaData;
        Str filename;
        enum ESnapshotDataType {
            SpaceSnapshot,
            TimeDFTSnapshot,
            SpaceDFTSnapshot,
            unknownSnapshot
        } snapshotDataType;

        using SnapshotDataType [[deprecated("Use ESnapshotDataType")]] = ESnapshotDataType;
    };

    class FSnapshotFileLoader {
        FSnapshotData snapshotData;

        static auto ReadPyDict(const Str& filePath) -> PythonUtils::PyDict;
        static auto ReadData(const Str& filePath) -> FRealVector;

    public:
        explicit FSnapshotFileLoader(const Str &filename);

        static auto Load(const Str &filename) -> FSnapshotData;
    };

    using SnapshotData [[deprecated("Use FSnapshotData")]] = FSnapshotData;
    using SnapshotFileLoader [[deprecated("Use FSnapshotFileLoader")]] = FSnapshotFileLoader;

} // Modes

#endif //STUDIOSLAB_SNAPSHOTFILELOADER_H
