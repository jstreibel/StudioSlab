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


namespace Modes::DatabaseViewer {
    using namespace Slab;

    typedef std::map<Real, std::shared_ptr<Math::RtoR::NumericFunction_CPU>> FieldMap;
    class DBParser {
        Str rootDatabaseFolder;
        std::map<Real, Str> fileSet;
        FieldMap fieldMap;
        Str criticalParameter;

        void readDatabase(const Str& folder);
        void checkIntervalConsistency();

    public:
        using Ptr = Pointer<Modes::DatabaseViewer::DBParser>;

        explicit DBParser(const Str& rootDBFolder, Str  criticalParameter);

        auto getCriticalParameter() const -> Str;
        auto getFileSet() const -> const std::map<Real, Str>&;
        auto getFieldMap() const -> const FieldMap &;

        auto getRootDatabaseFolder() const -> const Str&;

        auto buildFullField() const -> Pointer<Math::R2toR::NumericFunction_CPU>;
    };
} // Modes::DatabaseViewer

#endif //STUDIOSLAB_DATABASEPARSER_H
