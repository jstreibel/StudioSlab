//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_DATABASEPARSER_H
#define STUDIOSLAB_DATABASEPARSER_H

#include <map>

#include "Math/Function/RtoR/Model/RtoRDiscreteFunctionCPU.h"

#include "Utils/Types.h"
#include "Utils/PythonUtils.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunctionCPU.h"


namespace Modes::DatabaseViewer {
    typedef std::map<Real, std::shared_ptr<RtoR::DiscreteFunction_CPU>> FieldMap;
    class DBParser {
        Str rootDatabaseFolder;
        std::map<Real, Str> fileSet;
        FieldMap fieldMap;
        Str criticalParameter;

        void readDatabase(const Str& folder);
        void checkIntervalConsistency();

    public:
        typedef std::shared_ptr<Modes::DatabaseViewer::DBParser> Ptr;

        explicit DBParser(const Str& rootDBFolder, Str  criticalParameter);

        auto getCriticalParameter() const -> Str;
        auto getFileSet() const -> const std::map<Real, Str>&;
        auto getFieldMap() const -> const FieldMap &;

        auto getRootDatabaseFolder() const -> const Str&;

        auto buildFullField() const -> std::shared_ptr<R2toR::DiscreteFunction_CPU>;
    };
} // Modes::DatabaseViewer

#endif //STUDIOSLAB_DATABASEPARSER_H
