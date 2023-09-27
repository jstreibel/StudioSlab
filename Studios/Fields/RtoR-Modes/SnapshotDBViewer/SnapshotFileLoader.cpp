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

    SnapshotFileLoader::SnapshotFileLoader(const Str &filename) {
        fieldPtr = Load(filename);
    }

    auto SnapshotFileLoader::Load(const Str &filename) -> std::shared_ptr<RtoR::DiscreteFunction_CPU> {
        auto dict = ReadPyDict(filename);
        auto data = ReadData(filename);

        RealArray dataArr(data.data(), data.size());

        char *endPtr;
        auto N    = std::strtol(dict["N"].first.c_str(), &endPtr, 10);
        auto L    = std::strtod(dict["L"].first.c_str(), &endPtr);
        auto xMin = std::strtod(dict["xMin"].first.c_str(), &endPtr);
        auto xMax = xMin+L;

        if(filename.rfind(".dft.snapshot")) {
            if(!(dataArr.size() == N/2+1))
                Log::Error() << "Expected DFT array size was " << N/2+1 << ", found " << dataArr.size() << Log::Flush;

            fix Δk = 2 * Constants::pi / L;
            xMin = 0.0;
            xMax = Δk*(Real)dataArr.size();
        }

        auto field = new RtoR::DiscreteFunction_CPU(dataArr, xMin, xMax);

        return std::shared_ptr<RtoR::DiscreteFunction_CPU>{field};
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

        inFile.close();

        return doubleData;
    }

} // Modes