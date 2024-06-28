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
        fieldPtr = Load(filename);
    }

    auto SnapshotFileLoader::Load(const Str &filename) -> Pointer<Math::RtoR::NumericFunction_CPU> {
        auto dict = ReadPyDict(filename);
        auto data = ReadData(filename);

        RealArray dataArr(data.data(), data.size());

        char *endPtr;

        if(filename.rfind("time.dft.snapshot")) {
            auto t = std::strtod(dict["t"].first.c_str(), &endPtr);
            return New<Math::RtoR::NumericFunction_CPU>(dataArr, 0, t);
        } else if(filename.rfind(".dft.snapshot")) {
            auto L    = std::strtod(dict["L"].first.c_str(), &endPtr);
            auto xMin = std::strtod(dict["xMin"].first.c_str(), &endPtr);
            auto xMax = xMin+L;

            auto N    = std::strtol(dict["N"].first.c_str(), &endPtr, 10);

            if(dataArr.size() != N / 2 + 1)
                Log::Error() << "Expected DFT array size was " << N/2+1 << ", found " << dataArr.size() << Log::Flush;

            fix Δk = 2 * Math::Constants::pi / L;
            xMin = 0.0;
            xMax = Δk*(Real)dataArr.size();


            return New<Math::RtoR::NumericFunction_CPU>(dataArr, xMin, xMax);
        } else if(filename.rfind(".snapshot")) {
            auto L    = std::strtod(dict["L"].first.c_str(), &endPtr);
            auto xMin = std::strtod(dict["xMin"].first.c_str(), &endPtr);
            auto xMax = xMin+L;

            return New<Math::RtoR::NumericFunction_CPU>(dataArr, xMin, xMax);
        }

        Log::Error() << "Unknown format type " << filename << Log::Flush;
        NOT_IMPLEMENTED
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