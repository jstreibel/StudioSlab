//
// Created by joao on 26/09/23.
//

#include "HistoryFileLoader.h"

#include <fstream>
#include <iomanip>


namespace Modes {
    using namespace Slab;

    using Log = Core::Log;

    RealVector remove_first_column(RealVector &vec, Int N, Int M) {
        auto N_new = N-1;
        RealVector new_vec(N_new * M);

        // Iterate over each row
        for (int j = 0; j < M; ++j) {
            // Iterate over each column, starting from the second column (i.e., i = 1)
            for (int i = 1; i <= N; ++i) {
                // Copy the element from the original vector to the new vector
                new_vec[(i-1) + j * N_new] = vec[i + j * N];
            }
        }

        return new_vec;
    }

    auto HistoryFileLoader::Load(const Str &filename) -> Pointer<Math::R2toR::NumericFunction_CPU> {
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

        auto field = New<Math::R2toR::NumericFunction_CPU>(N, M, xMin, t0, hx, hy);

        if(data.size() != N*M) {
            Log::Error() << "data.size() should yield NxM = " << N << "x" << M << " = " << N*M << " but was " << data.size() << Log::Flush;
            throw Exception(Str("Assertion data.size()==N*M failed @ ") + __PRETTY_FUNCTION__ + ":" + ToStr(__LINE__));
        }

        for (int i=0; i<N; ++i) for (int j=0; j<M; ++j) field->At(i, j) = data[i + j*N];

        Log::Info() << "Instantiated and filled " << N << " x " << M << " NumericFunction_CPU with field data." << Log::Flush;

        return field;
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
        bool lines_contain_timestamp =
                lct == "True" || !(lct == "False");// TODO: && throw Exception("Unknown data type in .oscb file");
        assert(outresX.second == PythonUtils::Integer);
        assert(outresT.second == PythonUtils::Integer);
        long N = strtol(outresX.first.c_str(), nullptr, 10);
        long M = strtol(outresT.first.c_str(), nullptr, 10);

        size_t timestamp_size = lines_contain_timestamp ? 1 : 0;
        size_t data_count = (N+timestamp_size)*M;
        RealVector double_data(data_count);

        if(dataType == fp32){
            auto type_size = sizeof(float);
            auto data_size = (std::streamsize)(data_count*type_size);

            auto data = static_cast<RealData>(malloc(data_size));
            file.read(reinterpret_cast<char *>(data), data_size);

            for(auto i=0; i<data_count; ++i) double_data[i] = (double)data[i];
        } else if(dataType == fp64){
            auto type_size = sizeof(double);
            auto data_size = (std::streamsize)(data_count*type_size);

            file.read(reinterpret_cast<char *>(&double_data[0]), data_size);
        }

        if(lines_contain_timestamp)
            double_data = remove_first_column(double_data, N+1, M);

        return {&double_data[0], double_data.size()};
    }
} // Modes