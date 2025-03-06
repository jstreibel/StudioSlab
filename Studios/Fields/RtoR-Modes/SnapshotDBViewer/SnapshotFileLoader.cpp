//
// Created by joao on 26/09/23.
//

#include "SnapshotFileLoader.h"

#include "Math/Constants.h"
#include "Math/Data/DataAllocator.h"


#include <fstream>

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

        if(filename.ends_with("time.dft.simsnap")
        || filename.ends_with("time.dft.snapshot")) {
            // auto t = std::strtod(metaData["t"].first.c_str(), &endPtr);
            // auto dω = (2*M_PI/t);
            fix dω = std::strtod(metaData["dohm"].first.c_str(), &endPtr);
            fix n = dataArr.size();
            fix Δω = static_cast<Real>(n)*dω;
            Core::Log::Debug() << "Loaded time.dft.snapshot of Δω=" << Δω << Log::Flush;
            snapshotField = Math::DataAlloc<Math::RtoR::NumericFunction_CPU>("SnapshotField", dataArr, 0, Δω);
            snapshotDataType = SnapshotData::TimeDFTSnapshot;
        } else if(filename.ends_with(".dft.simsnap")
               || filename.ends_with(".dft.snapshot")) {
            fix L    = std::strtod(metaData["L"].first.c_str(), &endPtr);
            auto xMin = std::strtod(metaData["xMin"].first.c_str(), &endPtr);
            auto xMax = xMin+L;

            if(fix N = std::strtol(metaData["N"].first.c_str(), &endPtr, 10);
                dataArr.size() != N / 2 + 1)
                Log::Error() << "Expected DFT array size was " << N/2+1 << ", found " << dataArr.size() << Log::Flush;

            fix Δk = 2 * Math::Constants::pi / L;
            xMin = 0.0;
            xMax = Δk*static_cast<Real>(dataArr.size());


            snapshotField = Math::DataAlloc<Math::RtoR::NumericFunction_CPU>("SnapshotField", dataArr, xMin, xMax);
            snapshotDataType = SnapshotData::SpaceDFTSnapshot;
        } else if(filename.ends_with(".simsnap")
               || filename.ends_with(".snapshot")) {
            fix L    = std::strtod(metaData["L"].first.c_str(), &endPtr);
            auto xMin = std::strtod(metaData["xMin"].first.c_str(), &endPtr);
            auto xMax = xMin+L;

            snapshotField = Math::DataAlloc<Math::RtoR::NumericFunction_CPU>("SnapshotField", dataArr, xMin, xMax);
            snapshotDataType = SnapshotData::SpaceSnapshot;
        } else {
            Log::Error() << "Unknown format type " << filename << Log::Flush;
            throw Exception("Unknown file format");
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