//
// Created by joao on 22/09/23.
//

#include "DatabaseParser.h"


#include "SnapshotFileLoader.h"


#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <vector>
#include <filesystem>  // C++17 and above


namespace Modes::DatabaseViewer {

    using Log = Core::Log;

    class DirtyDBException : public Exception {
    public:
        explicit DirtyDBException(const Str &msg="") : Exception(Str("dirty snapshot database") + (!msg.empty()?": "+msg:""))  { }
    };

    [[nodiscard]]
    double parse_filename_for_critical_parameter_value(const Str& fileName, const Str& criticalParameter) {
        std::size_t pos = fileName.rfind(criticalParameter);
        if(pos == Str::npos)
            throw DirtyDBException(Str("Database file '") + fileName + "' "
                                       + "does not contain critical parameter '" + criticalParameter + "'; "
                                       + "database deemed dirty");

        // Find the position of the '=' character that follows the criticalParam
        auto equalsPos = pos + criticalParameter.length(); // fileName.find('=', pos + criticalParameter.length());
        if (equalsPos == std::string::npos)
            throw DirtyDBException("Could not find critical parameter '" +criticalParameter +
                                   "' in filename '" + fileName + "'.");

        // Extract the substring starting from the position after '='
        auto valueStr = fileName.substr(equalsPos + 1);

        // Find the end of the double-formatted value (assuming it ends with an underscore)
        StrVector terminators = {"time.dft.simsnap", ".dft.simsnap", ".simsnap", " "};
        Count endPos;
        for(auto &term : terminators) {
            endPos = valueStr.find(term);
            if(endPos != Str::npos) break;
        }
        if (endPos != Str::npos) valueStr = valueStr.substr(0, endPos);

        // Convert the string to a double
        double value;
        try {
            value = std::stod(valueStr);
        } catch (std::exception &e) {
            Core::Log::ErrorFatal() << "std::exception: " << e.what();
            Core::Log::Info() << "@" << __PRETTY_FUNCTION__ << ":" << __LINE__;
            Core::Log::Info() << "While trying to convert string '" << valueStr << "' to double.";
            Core::Log::Info() << "While processing file '" << fileName << "." << Core::Log::Flush;

            throw DirtyDBException();
        }

        return value;
    }

    [[nodiscard]]
    double get_scaling_for_critical_parameter(const Str &criticalParameter,
                                              const PythonUtils::PyDict& metaData) {

        char *endPtr;
        if(criticalParameter == "omega") return 1.0;
        if(criticalParameter == "omega_n") NOT_IMPLEMENTED
        if(criticalParameter == "harmonic") {
            //fix L = PythonUtils::Get<Real>(Str("L"), metaData);
            fix L = std::strtod(metaData.at("L").first.c_str(), &endPtr);
            fix dk = 2*M_PI/L;
            return dk; // k = n*dk = 2π/L * n
        }

        NOT_IMPLEMENTED
    }

    fix DefaultDFTSnapshotsFolder = "./snapshots/";
    fix DefaultHistoriesFolder = "./histories/";

    DBParser::DBParser(Str rootDBFolder, Str criticalParameter)
    : criticalParameter(std::move(criticalParameter))
    , rootDatabaseFolder(std::move(rootDBFolder))
    {
        if(rootDatabaseFolder=="./" || rootDatabaseFolder=="."){
            fix pwd = Common::GetPWD();
            fix splits = Common::SplitString(pwd, "/");
            fix n = splits.size();
            rootDatabaseFolder = "../" + (splits[n-1] + "/");
        }

        Log::Info() << "Parsing database from " << rootDatabaseFolder << Log::Flush;

        readDatabase();

        checkIntervalConsistency();
    }

    void DBParser::checkIntervalConsistency() {
        RealVector values;

        for(auto &entry : fileSet) {
            values.emplace_back(entry.first);

            // ReadPyDict(entry.second);
        }

        fix N = values.size();
        Real lastDelta = values[1]-values[0];

        fix eps = 1.e-3*(values[N-1]-values[N-2]);

        for(int i=1; i<N-1; ++i){
            Real delta = values[i+1]-values[i];

            if(!Common::AreEqual(lastDelta, delta, eps)) {
                Log::Fail() << "DBParser deltas differ for "
                << criticalParameter << "=" << values[i-1] << "; "
                << criticalParameter << "=" << values[i] << "; " << criticalParameter << "=" << values[i+1]
                << ": lastDelta-delta = " << lastDelta << "-" << delta << " = " << lastDelta-delta << "; tolerance ε=" << eps << Log::Flush;
                // throw DirtyDBException();
            }

            lastDelta = delta;
        }
    }

    void DBParser::readDatabase() {
        auto dbPath = rootDatabaseFolder;

        auto snapshotsFolderIterator = std::filesystem::directory_iterator(dbPath + "/" + DefaultDFTSnapshotsFolder);

        for (const auto &entry: snapshotsFolderIterator) {
            auto fileName = entry.path().string();

            auto value = parse_filename_for_critical_parameter_value(fileName, criticalParameter);

            if(!fileSet[value].empty()) {
                Log::Error() << "Value " << criticalParameter << "=" << value << " already exsists. "
                             << "Conflicting files are \""
                             << Log::FGRed << fileSet[value] << Log::ResetFormatting << "\" and \""
                             << Log::FGRed << fileName << Log::ResetFormatting << "\"" << Log::Flush;
                throw DirtyDBException();
            }

            auto snapshotData = SnapshotFileLoader::Load(fileName);
            fix scaling = get_scaling_for_critical_parameter(criticalParameter, snapshotData.metaData);

            auto snapshotEntry = SnapshotEntry{snapshotData, value, criticalParameter, scaling};

            fileSet[value] = fileName;
            fieldMap[value] = snapshotEntry;
        }

        Log::Success() << "Extracted critical parameter '" << criticalParameter
                       << "' from database. Relation is below." << Log::Flush;

        for(auto &entry : fileSet)
            Log::Debug() << "Extracted value " << criticalParameter << "=" << std::left << std::setw(10) << entry.first << " from "
                        << entry.second << Log::Flush;
    }

    auto DBParser::getFileSet()           const -> const std::map<Real, Str> & { return fileSet; }
    auto DBParser::getCriticalParameter() const -> Str { return criticalParameter; }

    auto DBParser::buildFullField() const -> std::shared_ptr<Math::R2toR::NumericFunction_CPU> {
        IN sampleField = *fieldMap.begin()->second.snapshotData.data;

        fix N = fieldMap.size();
        fix M = sampleField.N;
        fix ωMin = fieldMap.begin() ->second.getScaledCriticalParameter();
        fix ωMax = fieldMap.rbegin()->second.getScaledCriticalParameter();
        fix kMin = sampleField.xMin;
        fix kMax = sampleField.xMax;
        fix hω = (ωMax-ωMin)/(Real)N;
        fix hk = (kMax-kMin)/(Real)M;


        auto fullField = new Math::R2toR::NumericFunction_CPU(N, M, ωMin, kMin, hω, hk);

        int i=0;
        for (auto &pair: fieldMap) {
            IN field = *pair.second.snapshotData.data;
            auto &spaceData = field.getSpace().getHostData();
            for(int j=0; j<M; ++j)
                fullField->At(i, j) = spaceData[j];
            ++i;
        }

        return std::shared_ptr<Math::R2toR::NumericFunction_CPU>(fullField);
    }

    auto DBParser::getFieldMap() const -> const FieldMap & { return fieldMap; }

    auto DBParser::getRootDatabaseFolder() const -> const Str & { return rootDatabaseFolder; }

    DatabaseType DBParser::evaluateDatabaseType() const {
        fix baseSnapshotType = fieldMap.begin()->second.snapshotData.snapshotDataType;

        for(IN entry : fieldMap) {
            fix snapshotType = entry.second.snapshotData.snapshotDataType;

            if(snapshotType != baseSnapshotType) return MixedDBType;
        }

        switch (baseSnapshotType) {
            case SnapshotData::SpaceSnapshot:       return SpaceDBType;
            case SnapshotData::TimeDFTSnapshot:     return TimeDFTDBType;
            case SnapshotData::SpaceDFTSnapshot:    return SpaceDFTDBType;
            case SnapshotData::unknownSnapshot: default: ;
        }

        return unknownDBType;
    }


} // Modes::DatabaseViewer