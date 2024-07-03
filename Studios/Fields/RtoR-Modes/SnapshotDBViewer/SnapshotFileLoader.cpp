//
// Created by joao on 26/09/23.
//

#include "SnapshotFileLoader.h"

#include "Utils/Exception.h"
#include "Utils/PythonUtils.h"
#include "Core/Tools/Log.h"
#include "Math/Constants.h"


#include <fstream>
#include <iomanip>

namespace Modes {

    using Log = Core::Log;

    SnapshotFileLoader::SnapshotFileLoader(const Str &filename) {
        snapshotData = Load(filename);
    }

    auto SnapshotFileLoader::Load(const Str &filename) -> SnapshotData {
        auto metaData = ReadPyDict(filename);
        auto data = ReadData(filename);

        RealArray dataArr(data.data(), data.size());

        char *endPtr;

        Pointer<Math::RtoR::NumericFunction_CPU> snapshotField;

        SnapshotData::SnapshotDataType snapshotDataType = SnapshotData::unknownSnapshot;

        if(filename.rfind("time.dft.snapshot")) {
            // auto t = std::strtod(metaData["t"].first.c_str(), &endPtr);
            // auto dω = (2*M_PI/t);
            auto dω = std::strtod(metaData["dohm"].first.c_str(), &endPtr);
            auto n = dataArr.size();
            auto Δω = Real(n)*dω;
            Core::Log::Debug() << "Loaded time.dft.snapshot of Δω=" << Δω << Log::Flush;
            snapshotField = New<Math::RtoR::NumericFunction_CPU>(dataArr, 0, Δω);
            snapshotDataType = SnapshotData::TimeDFTSnapshot;
        } else if(filename.rfind(".dft.snapshot")) {
            auto L    = std::strtod(metaData["L"].first.c_str(), &endPtr);
            auto xMin = std::strtod(metaData["xMin"].first.c_str(), &endPtr);
            auto xMax = xMin+L;

            auto N    = std::strtol(metaData["N"].first.c_str(), &endPtr, 10);

            if(dataArr.size() != N / 2 + 1)
                Log::Error() << "Expected DFT array size was " << N/2+1 << ", found " << dataArr.size() << Log::Flush;

            fix Δk = 2 * Math::Constants::pi / L;
            xMin = 0.0;
            xMax = Δk*(Real)dataArr.size();


            snapshotField = New<Math::RtoR::NumericFunction_CPU>(dataArr, xMin, xMax);
            snapshotDataType = SnapshotData::SpaceDFTSnapshot;
        } else if(filename.rfind(".snapshot")) {
            auto L    = std::strtod(metaData["L"].first.c_str(), &endPtr);
            auto xMin = std::strtod(metaData["xMin"].first.c_str(), &endPtr);
            auto xMax = xMin+L;

            snapshotField = New<Math::RtoR::NumericFunction_CPU>(dataArr, xMin, xMax);
            snapshotDataType = SnapshotData::SpaceSnapshot;
        } else {
            Log::Error() << "Unknown format type " << filename << Log::Flush;
            NOT_IMPLEMENTED
        }

        return {snapshotField, metaData, filename, snapshotDataType};
    }

    auto SnapshotFileLoader::ReadPyDict(const Str& filePath) -> PythonUtils::PyDict {
        std::ifstream inFile(filePath, std::ios::binary);

        if (!inFile) throw Exception(Str("Error opening file '") + filePath + "'");

        Str line;
        if(std::getline(inFile, line))
            if(!PythonUtils::BadPythonDictionary(line))
                return PythonUtils::ParsePythonDict(line);

        Log::Fail() << "First line is not a Python dictionary" << Log::Flush;
        throw Exception("file \"" + filePath + "\" does not contain Python dictionary header");
    }

    auto SnapshotFileLoader::ReadData(const Str& filePath) -> RealVector {
        std::ifstream inFile(filePath, std::ios::binary);

        if (!inFile) throw Exception(Str("Error opening file '") + filePath + "'");

        // Read text lines until separator is found
        Str line;
        Str separator = "<<<<-oOo->>>>";
        while (std::getline(inFile, line)) if (line == separator) break;

        // Read binary data into a vector of doubles
        RealVector doubleData;
        double value;
        while (inFile.read(reinterpret_cast<char*>(&value), sizeof(double)))
            doubleData.emplace_back(value);

        /*
        RealVector doubleData(count);
        inFile.read(reinterpret_cast<char*>(doubleData.data()), count*sizeof(double));
        */

        inFile.close();

        return doubleData;
    }

} // Modes