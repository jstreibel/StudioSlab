//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_DATABASEPARSER_H
#define STUDIOSLAB_DATABASEPARSER_H

#include <map>

#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"

#include "Utils/Types.h"
#include "Utils/PythonUtils.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "SnapshotFileLoader.h"


namespace Modes::DatabaseViewer {
    using namespace Slab;

    fix DefaultDFTSnapshotsFolder = "./snapshots/";
    fix DefaultHistoriesFolder = "./histories/";

    struct FSnapshotEntry {
        FSnapshotData snapshotData;

        DevFloat critical_parameter_value;
        Str critical_parameter_name;
        DevFloat scaling;

        DevFloat getScaledCriticalParameter() const { return critical_parameter_value*scaling; }
    };

    enum EDatabaseType {
        SpaceDFTDBType,
        TimeDFTDBType,
        SpaceDBType,
        MixedDBType,
        unknownDBType
    };

    using SnapshotEntry [[deprecated("Use FSnapshotEntry")]] = FSnapshotEntry;
    using DatabaseType [[deprecated("Use EDatabaseType")]] = EDatabaseType;

    using FieldMap = std::map<DevFloat, FSnapshotEntry>;

    class FDBParser {
        Str rootDatabaseFolder;
        Str snapshotFolder;
        std::map<DevFloat, Str> fileSet;
        FieldMap fieldMap;
        Str criticalParameter;

        void readDatabase();
        void checkIntervalConsistency() const;

    public:
        using Ptr = TPointer<Modes::DatabaseViewer::FDBParser>;

        explicit FDBParser(Str rootDBFolder,
                          Str  criticalParameter,
                          Str snapshotsFolder=DefaultDFTSnapshotsFolder);

        auto getCriticalParameter() const -> Str;
        auto getFileSet() const -> const std::map<DevFloat, Str>&;
        auto getSnapshotMap() const -> const FieldMap &;

        EDatabaseType evaluateDatabaseType() const;

        auto getRootDatabaseFolder() const -> const Str&;

        auto buildSnapshotMashup() const -> TPointer<Math::R2toR::NumericFunction_CPU>;

    };

    using DBParser [[deprecated("Use FDBParser")]] = FDBParser;
} // Modes::DatabaseViewer

#endif //STUDIOSLAB_DATABASEPARSER_H
