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

    namespace DatabaseViewer { class DBParser; }

    struct SnapshotData {
        TPointer<Math::RtoR::NumericFunction_CPU> data;
        PythonUtils::PyDict metaData;
        Str filename;
        enum SnapshotDataType {
            SpaceSnapshot,
            TimeDFTSnapshot,
            SpaceDFTSnapshot,
            unknownSnapshot
        } snapshotDataType;
    };

    class SnapshotFileLoader {
        SnapshotData snapshotData;

        static auto ReadPyDict(const Str& filePath) -> PythonUtils::PyDict;
        static auto ReadData(const Str& filePath) -> FRealVector;

    public:
        explicit SnapshotFileLoader(const Str &filename);

        static auto Load(const Str &filename) -> SnapshotData;
    };

} // Modes

#endif //STUDIOSLAB_SNAPSHOTFILELOADER_H
