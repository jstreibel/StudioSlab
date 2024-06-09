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

    class DirtyDBException : public Exception {
    public:
        explicit DirtyDBException(const Str &msg="") : Exception(Str("dirty snapshot database") + (!msg.empty()?": "+msg:""))  { }
    };

    fix DefaultDFTSnapshotsFolder = "./snapshots/";
    fix DefaultHistoriesFolder = "./histories/";

    DBParser::DBParser(const Str& rootDBFolder, Str  criticalParameter)
    : criticalParameter(std::move(criticalParameter))
    , rootDatabaseFolder(rootDBFolder)
    {
        Log::Info() << "Parsing database from " << rootDBFolder << Log::Flush;

        readDatabase(rootDBFolder);

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

            if(!Common::areEqual(lastDelta, delta, eps)) {
                Log::Fail() << "DBParser deltas differ for "
                << criticalParameter << "=" << values[i-1] << "; "
                << criticalParameter << "=" << values[i] << "; " << criticalParameter << "=" << values[i+1]
                << ": lastDelta-delta = " << lastDelta << "-" << delta << " = " << lastDelta-delta << "; tolerance ε=" << eps << Log::Flush;
                // throw DirtyDBException();
            }

            lastDelta = delta;
        }
    }

    void DBParser::readDatabase(const Str& dbPath) {
        auto snapshotsFolderIterator = std::filesystem::directory_iterator(dbPath + "/" + DefaultDFTSnapshotsFolder);

        for (const auto &entry: snapshotsFolderIterator) {
            auto fileName = entry.path().string();

            std::size_t pos = fileName.rfind(criticalParameter);
            if (pos != Str::npos) {
                // Find the position of the '=' character that follows the criticalParam
                auto equalsPos = pos + criticalParameter.length(); // fileName.find('=', pos + criticalParameter.length());
                if (equalsPos != std::string::npos) {
                    // Extract the substring starting from the position after '='
                    auto valueStr = fileName.substr(equalsPos + 1);

                    // Find the end of the double-formatted value (assuming it ends with an underscore)
                    StrVector terminators = {".dft.simsnap", ".simsnap", " "};
                    Count endPos;
                    for(auto &term : terminators) {
                        endPos = valueStr.find(term);
                        if(endPos != Str::npos) break;
                    }
                    if (endPos != Str::npos) valueStr = valueStr.substr(0, endPos);

                    // Convert the string to a double
                    double value = std::stod(valueStr);

                    if(!fileSet[value].empty()) {
                        Log::Error() << "Value " << criticalParameter << "=" << value << " already exsists. "
                        << "Conflicting files are \""
                        << Log::FGRed << fileSet[value] << Log::ResetFormatting << "\" and \""
                        << Log::FGRed << fileName << Log::ResetFormatting << "\"" << Log::Flush;
                        throw DirtyDBException();
                    }

                    fileSet[value] = fileName;
                    fieldMap[value] = SnapshotFileLoader::Load(fileName);
                }

            } else {
                Log::Error() << "Database file '" << fileName << "' "
                             << "does not contain critical parameter '" << criticalParameter << "'; "
                             << "database deemed dirty" << Log::Flush;

                throw DirtyDBException();
            }
        }

        Log::Success() << "Extracted critical parameter '" << criticalParameter
                       << "' from database. Relation is below." << Log::Flush;

        for(auto &entry : fileSet)
            Log::Debug() << "Extracted value " << criticalParameter << "=" << std::left << std::setw(10) << entry.first << " from "
                        << entry.second << Log::Flush;
    }

    auto DBParser::getFileSet()           const -> const std::map<Real, Str> & { return fileSet; }
    auto DBParser::getCriticalParameter() const -> Str { return criticalParameter; }

    auto DBParser::buildFullField() const -> std::shared_ptr<R2toR::NumericFunction_CPU> {
        IN sampleField = *fieldMap.begin()->second;

        fix N = fieldMap.size();
        fix M = sampleField.N;
        fix ωMin = fieldMap.begin() ->first;
        fix ωMax = fieldMap.rbegin()->first;
        fix kMin = sampleField.xMin;
        fix kMax = sampleField.xMax;
        fix hω = (ωMax-ωMin)/(Real)N;
        fix hk = (kMax-kMin)/(Real)M;


        auto fullField = new R2toR::NumericFunction_CPU(N, M, ωMin, kMin, hω, hk);

        int i=0;
        for (auto &pair: fieldMap) {
            auto ω = pair.first;
            IN field = *pair.second;
            auto &spaceData = field.getSpace().getHostData();
            for(int j=0; j<M; ++j)
                fullField->At(i, j) = spaceData[j];
            ++i;
        }

        return std::shared_ptr<R2toR::NumericFunction_CPU>(fullField);
    }

    auto DBParser::getFieldMap() const -> const FieldMap & { return fieldMap; }

    auto DBParser::getRootDatabaseFolder() const -> const Str & { return rootDatabaseFolder; }


} // Modes::DatabaseViewer