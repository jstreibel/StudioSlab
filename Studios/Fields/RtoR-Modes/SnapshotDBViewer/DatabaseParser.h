//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_DATABASEPARSER_H
#define STUDIOSLAB_DATABASEPARSER_H

#include "Utils/Types.h"
#include "Utils/PythonUtils.h"

#include <map>

namespace Modes::DatabaseViewer {
    class DBParser {
        std::map<Real, Str> fileSet;
        Str criticalParameter;

        void readFolder(const Str& folder);
        void checkIntervalConsistency();

        static PythonUtils::PyDict ReadPyDict(const Str& filename);
        static RealVector ReadData(Str filename);
    public:
        typedef std::shared_ptr<Modes::DatabaseViewer::DBParser> Ptr;

        explicit DBParser(const Str& rootDBFolder, const Str& criticalParameter);

        auto getCriticalParameter() const -> Str;
        auto getFileSet() const -> const std::map<Real, Str>&;
    };
} // Modes::DatabaseViewer

#endif //STUDIOSLAB_DATABASEPARSER_H
