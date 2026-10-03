//
// Created by joao on 26/09/23.
//

#include "HistoryFileLoader.h"

#include <cctype>
#include <filesystem>
#include <limits>

namespace {
using namespace Slab;

auto HasOnlyDictionarySuffix(const char* remainder) -> bool {
    while (*remainder != '\0' && std::isspace(static_cast<unsigned char>(*remainder)))
        ++remainder;
    if (*remainder == '}') {
        ++remainder;
        while (*remainder != '\0' && std::isspace(static_cast<unsigned char>(*remainder)))
            ++remainder;
    }
    return *remainder == '\0';
}

auto RequireValue(const PythonUtils::PyDict& dictionary, const Str& key) -> const PythonUtils::Value& {
    const auto value = dictionary.find(key);
    if (value == dictionary.end())
        throw Exception("OSCB header is missing required key '" + key + "'.");
    return value->second;
}

auto ReadLong(const PythonUtils::PyDict& dictionary, const Str& key) -> long {
    const auto& value = RequireValue(dictionary, key).first;
    char* end = nullptr;
    const auto parsed = std::strtol(value.c_str(), &end, 10);
    if (end == value.c_str() || !HasOnlyDictionarySuffix(end))
        throw Exception("OSCB header key '" + key + "' is not an integer.");
    return parsed;
}

auto ReadReal(const PythonUtils::PyDict& dictionary, const Str& key) -> DevFloat {
    const auto& value = RequireValue(dictionary, key).first;
    char* end = nullptr;
    const auto parsed = std::strtod(value.c_str(), &end);
    if (end == value.c_str() || !HasOnlyDictionarySuffix(end))
        throw Exception("OSCB header key '" + key + "' is not numeric.");
    return parsed;
}

auto ReadOptionalReal(const PythonUtils::PyDict& dictionary, const Str& preferredKey, const Str& compatibilityKey,
                      DevFloat fallback) -> DevFloat {
    if (dictionary.contains(preferredKey))
        return ReadReal(dictionary, preferredKey);
    if (dictionary.contains(compatibilityKey))
        return ReadReal(dictionary, compatibilityKey);
    return fallback;
}

auto ReadBool(const PythonUtils::PyDict& dictionary, const Str& key) -> bool {
    const auto& value = RequireValue(dictionary, key).first;
    if (value == "True")
        return true;
    if (value == "False")
        return false;
    throw Exception("OSCB header key '" + key + "' is not a boolean.");
}

auto CheckedProduct(size_t lhs, size_t rhs, const Str& description) -> size_t {
    if (lhs != 0 && rhs > std::numeric_limits<size_t>::max() / lhs)
        throw Exception("OSCB " + description + " exceeds addressable size.");
    return lhs * rhs;
}
} // namespace

namespace Modes {
using namespace Slab;

using Log = Core::Log;

auto FHistoryFileLoader::Load(const Str& filename) -> FLoadedHistory {
    const auto baseMessage = Str("Error opening file '") + filename + "'";

    if (!std::filesystem::exists(filename))
        throw Exception(baseMessage + ": file does not exist.");

    std::ifstream file(filename, std::ios::binary);
    if (!file)
        throw Exception(baseMessage + ": unable to open for binary reading.");

    auto metadata = ReadPyDict(file);
    auto decoded = ReadData(file, metadata);

    const auto totalTime = ReadReal(metadata, "t");
    const auto length = ReadReal(metadata, "L");
    const auto xCenter = ReadOptionalReal(metadata, "xCenter", "xcenter", 0.0);
    const auto xMin = xCenter - 0.5 * length;
    const auto hx = length / static_cast<DevFloat>(decoded.N);
    const auto ht = totalTime / static_cast<DevFloat>(decoded.M);

    auto phi = Math::DataAlloc<Math::R2toR::NumericFunction_CPU>("ϕ(t,x)", static_cast<UInt>(decoded.N),
                                                                 static_cast<UInt>(decoded.M), xMin, 0.0, hx, ht);

    for (long j = 0; j < decoded.M; ++j)
        for (long i = 0; i < decoded.N; ++i)
            phi->At(static_cast<UInt>(i), static_cast<UInt>(j)) = decoded.Phi[static_cast<size_t>(i + j * decoded.N)];

    TPointer<Math::R2toR::NumericFunction_CPU> dPhiDt;
    if (decoded.Channels == 2) {
        dPhiDt = Math::DataAlloc<Math::R2toR::NumericFunction_CPU>("∂ϕ/∂t(t,x)", static_cast<UInt>(decoded.N),
                                                                   static_cast<UInt>(decoded.M), xMin, 0.0, hx, ht);

        for (long j = 0; j < decoded.M; ++j)
            for (long i = 0; i < decoded.N; ++i)
                dPhiDt->At(static_cast<UInt>(i), static_cast<UInt>(j)) =
                    decoded.DPhiDt[static_cast<size_t>(i + j * decoded.N)];
    }

    Log::Info() << "Loaded " << filename << " as " << decoded.N << " x " << decoded.M << " OSCB history with "
                << decoded.Channels << " channel(s)." << Log::Flush;

    return FLoadedHistory{
        .Phi = std::move(phi),
        .DPhiDt = std::move(dPhiDt),
        .Timestamps = std::move(decoded.Timestamps),
        .MetaData = std::move(metadata),
    };
}

auto FHistoryFileLoader::ReadPyDict(std::ifstream& file) -> PythonUtils::PyDict {
    Str line;
    if (std::getline(file, line))
        return PythonUtils::ParsePythonDict(line);

    throw Exception("OSCB file does not contain a readable dictionary header.");
}

auto FHistoryFileLoader::ReadData(std::ifstream& file, const PythonUtils::PyDict& pyDict) -> FDecodedData {
    const auto N = ReadLong(pyDict, "outresX");
    const auto M = ReadLong(pyDict, "outresT");
    const auto channels = pyDict.contains("data_channels") ? ReadLong(pyDict, "data_channels") : 1;
    const auto containsTimestamp = ReadBool(pyDict, "lines_contain_timestamp");

    if (N <= 0 || M <= 0)
        throw Exception("OSCB dimensions outresX and outresT must be positive.");
    if (channels != 1 && channels != 2)
        throw Exception("Unsupported OSCB data channel count " + ToStr(channels) + "; expected 1 or 2.");
    if (static_cast<unsigned long>(N) > std::numeric_limits<UInt>::max() ||
        static_cast<unsigned long>(M) > std::numeric_limits<UInt>::max())
        throw Exception("OSCB dimensions exceed the NumericFunction index range.");

    const auto& dataTypeName = RequireValue(pyDict, "data_type").first;
    const auto dataType = dataTypeName == "fp32" ? fp32
                          : dataTypeName == "fp64"
                              ? fp64
                              : throw Exception("Unknown data type '" + dataTypeName + "' in OSCB file.");

    const auto timestampElements = containsTimestamp ? size_t{1} : size_t{0};
    const auto channelElements = CheckedProduct(static_cast<size_t>(N), static_cast<size_t>(channels), "row width");
    const auto rowElements = timestampElements + channelElements;
    const auto totalElements = CheckedProduct(rowElements, static_cast<size_t>(M), "payload element count");
    const auto elementSize = dataType == fp32 ? sizeof(float) : sizeof(double);
    const auto expectedBytes = CheckedProduct(totalElements, elementSize, "payload byte count");

    const auto payloadStart = file.tellg();
    if (payloadStart < 0)
        throw Exception("Unable to locate the OSCB payload.");
    file.seekg(0, std::ios::end);
    const auto payloadEnd = file.tellg();
    if (payloadEnd < payloadStart)
        throw Exception("Invalid OSCB payload bounds.");
    const auto availableBytes = static_cast<size_t>(payloadEnd - payloadStart);
    if (availableBytes != expectedBytes)
        throw Exception("OSCB payload size mismatch: expected " + ToStr(expectedBytes) + " bytes, found " +
                        ToStr(availableBytes) + ".");
    file.seekg(payloadStart);

    FRealVector values(totalElements);
    if (dataType == fp32) {
        Vector<float> input(totalElements);
        file.read(reinterpret_cast<char*>(input.data()), static_cast<std::streamsize>(expectedBytes));
        for (size_t i = 0; i < totalElements; ++i)
            values[i] = static_cast<DevFloat>(input[i]);
    } else {
        Vector<double> input(totalElements);
        file.read(reinterpret_cast<char*>(input.data()), static_cast<std::streamsize>(expectedBytes));
        for (size_t i = 0; i < totalElements; ++i)
            values[i] = static_cast<DevFloat>(input[i]);
    }
    if (!file)
        throw Exception("Failed while reading the OSCB payload.");

    FDecodedData decoded;
    decoded.N = N;
    decoded.M = M;
    decoded.Channels = channels;
    decoded.Timestamps.resize(static_cast<size_t>(M));
    decoded.Phi.resize(CheckedProduct(static_cast<size_t>(N), static_cast<size_t>(M), "field size"));
    if (channels == 2)
        decoded.DPhiDt.resize(decoded.Phi.size());

    for (long j = 0; j < M; ++j) {
        const auto row = static_cast<size_t>(j) * rowElements;
        auto cursor = row;
        decoded.Timestamps[static_cast<size_t>(j)] = containsTimestamp ? values[cursor++] : static_cast<DevFloat>(j);

        for (long i = 0; i < N; ++i)
            decoded.Phi[static_cast<size_t>(i + j * N)] = values[cursor++];
        if (channels == 2)
            for (long i = 0; i < N; ++i)
                decoded.DPhiDt[static_cast<size_t>(i + j * N)] = values[cursor++];
    }

    return decoded;
}
} // namespace Modes
