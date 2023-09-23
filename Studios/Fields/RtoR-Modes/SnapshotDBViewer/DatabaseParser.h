//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_DATABASEPARSER_H
#define STUDIOSLAB_DATABASEPARSER_H

#include <map>

#include "Maps/RtoR/Model/RtoRDiscreteFunctionCPU.h"

#include "Utils/Types.h"
#include "Utils/PythonUtils.h"



namespace Modes::DatabaseViewer {
    typedef std::map<Real, std::shared_ptr<RtoR::DiscreteFunction_CPU>> FieldMap;
    class DBParser {
        std::map<Real, Str> fileSet;
        FieldMap fieldMap;
        Str criticalParameter;

        void readFolder(const Str& folder);
        void checkIntervalConsistency();

        static auto ReadPyDict(const Str& filename) -> PythonUtils::PyDict;
        static auto ReadData(const Str& filename) -> RealVector;
        static auto BuildField(const Str& filename) -> std::shared_ptr<RtoR::DiscreteFunction_CPU>;

    public:
        typedef std::shared_ptr<Modes::DatabaseViewer::DBParser> Ptr;

        explicit DBParser(const Str& rootDBFolder, const Str& criticalParameter);

        auto getCriticalParameter() const -> Str;
        auto getFileSet() const -> const std::map<Real, Str>&;
        auto getFieldMap() const -> FieldMap;
    };
} // Modes::DatabaseViewer

#endif //STUDIOSLAB_DATABASEPARSER_H
