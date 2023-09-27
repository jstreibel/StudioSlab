//
// Created by joao on 26/09/23.
//

#include "HistoryFileLoader.h"

#include <fstream>
#include <iomanip>


namespace Modes {
    std::unique_ptr<R2toR::DiscreteFunction_CPU> HistoryFileLoader::Load(const Str &filename) {
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) throw Exception(Str("Error opening file '") + filename + "'");

        auto pyDict = ReadPyDict(inFile);
        auto data = ReadData(inFile, pyDict);

        inFile.close();

        Log::Info() << "Loaded " << filename << Log::Flush;

        auto &outresX = pyDict["outresX"];
        auto &outresT = pyDict["outresT"];
        assert(outresX.second == PythonUtils::Integer);
        assert(outresT.second == PythonUtils::Integer);
        auto N = strtol(outresX.first.c_str(), nullptr, 10);
        auto M = strtol(outresT.first.c_str(), nullptr, 10);
        auto NxM = N*M;

        auto t0 = 0.0;
        auto t = strtod(pyDict["t"].first.c_str(), nullptr);
        auto L = strtod(pyDict["L"].first.c_str(), nullptr);
        auto xCenter = strtod(pyDict["xCenter"].first.c_str(), nullptr);
        auto xMin = xCenter - .5*L;
        auto xMax = xCenter + .5*L;
        auto hx = L/(Real)N;
        auto hy = t/(Real)N;

        auto *field = new R2toR::DiscreteFunction_CPU(N, M, xMin, t0, hx, hy);

        for (int i=0; i<N; ++i) for (int j=0; j<M; ++j) field->At(i, j) = data[i + j*N];

        return std::unique_ptr<R2toR::DiscreteFunction_CPU>{field};
    }

    auto HistoryFileLoader::ReadPyDict(std::ifstream &file) -> PythonUtils::PyDict {
        Str line;
        if (std::getline(file, line))
            if(!PythonUtils::BadPythonDictionary(line))
                return PythonUtils::ParsePythonDict(line);

        Log::Fail() << "First line is not a Python dictionary" << Log::Flush;
        throw Exception("file does not contain Python dictionary header");
    }

    auto HistoryFileLoader::ReadData(std::ifstream &file, PythonUtils::PyDict pyDict) -> RealArray {
        IN outresX = pyDict["outresX"];
        IN outresT = pyDict["outresT"];
        IN dataType = pyDict["data_type"].first=="fp32" ? fp32 : pyDict["data_type"].first=="fp64" ? fp64 : throw Exception("Unknown data type in .oscb file");
        IN lct = pyDict["lines_contain_timestamp"].first;
        bool lines_contain_timestamp = lct == "True" ? true : lct == "False" ? false : throw Exception("Unknown data type in .oscb file");
        assert(outresX.second == PythonUtils::Integer);
        assert(outresT.second == PythonUtils::Integer);
        long N = strtol(outresX.first.c_str(), nullptr, 10);
        long M = strtol(outresT.first.c_str(), nullptr, 10);

        long NxM = N*M;
        RealVector doubleData(NxM);
        if(dataType == fp32){
            auto typeSize = sizeof(float);
            long dataSize = NxM*typeSize;

            auto data = static_cast<RealData>(malloc(dataSize));
            file.read(reinterpret_cast<char *>(data), dataSize);

            for(auto i=0; i<NxM; ++i) doubleData[i] = (double)data[i];
        } else if(dataType == fp64){
            auto typeSize = sizeof(double);
            long dataSize = NxM*typeSize;

            file.read(reinterpret_cast<char *>(&doubleData[0]), dataSize);
        }

        if(lines_contain_timestamp) {
            int count = 0;
            doubleData.erase(std::remove_if(doubleData.begin(), doubleData.end(),
            [&count, N](double) {
                return count++ % N == 0;
            }), doubleData.end());
        }

        return RealArray(&doubleData[0], doubleData.size());
    }
} // Modes