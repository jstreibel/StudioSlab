#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../../Studios/Fields/Viewers/HistoryFileLoader.h"

#include <atomic>
#include <filesystem>
#include <fstream>

namespace {
using Catch::Approx;

struct FTemporaryOscbFile {
    std::filesystem::path Path;

    explicit FTemporaryOscbFile(const std::string& name) {
        static std::atomic_uint Counter = 0;
        Path =
            std::filesystem::temp_directory_path() / ("studioslab-" + name + "-" + std::to_string(Counter++) + ".oscb");
    }

    ~FTemporaryOscbFile() {
        std::error_code error;
        std::filesystem::remove(Path, error);
    }
};

template <typename T>
void WriteOscb(const std::filesystem::path& path, const std::string& header, const std::vector<T>& payload) {
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    output << header << '\n';
    output.write(reinterpret_cast<const char*>(payload.data()),
                 static_cast<std::streamsize>(payload.size() * sizeof(T)));
    REQUIRE(output.good());
}
} // namespace

TEST_CASE("OSCB loader decodes stored phi and ddtphi channels", "[OSCB][History]") {
    FTemporaryOscbFile file("two-channel");
    const std::string header =
        R"(# {"Ver": 4, "lines_contain_timestamp": True, "outresT": 2, "outresX": 3, "data_type": "fp32", "data_channels": 2, "data_channel_names": ("phi", "ddtphi"), "t": 1.0, "L": 6.0, "xCenter": 1.0})";
    const std::vector<float> payload{
        0.0F, 1.0F, 2.0F, 3.0F, 10.0F, 20.0F, 30.0F, 2.0F, 4.0F, 5.0F, 6.0F, 40.0F, 50.0F, 60.0F,
    };
    WriteOscb(file.Path, header, payload);

    const auto history = Modes::FHistoryFileLoader::Load(file.Path.string());

    REQUIRE(history.HasStoredTimeDerivative());
    REQUIRE(history.Timestamps == Slab::FRealVector{0.0, 2.0});
    REQUIRE(history.Phi->At(0, 0) == Approx(1.0));
    REQUIRE(history.Phi->At(2, 1) == Approx(6.0));
    REQUIRE(history.DPhiDt->At(0, 0) == Approx(10.0));
    REQUIRE(history.DPhiDt->At(2, 1) == Approx(60.0));
}

TEST_CASE("OSCB loader preserves legacy one-channel files", "[OSCB][History]") {
    FTemporaryOscbFile file("one-channel");
    const std::string header =
        R"(# {"Ver": 4, "lines_contain_timestamp": True, "outresT": 2, "outresX": 2, "data_type": "fp64", "data_channels": 1, "data_channel_names": ("phi",), "t": 2.0, "L": 4.0, "xcenter": 0.5})";
    const std::vector<double> payload{
        0.0, 1.25, 2.5, 4.0, 3.75, 5.0,
    };
    WriteOscb(file.Path, header, payload);

    const auto history = Modes::FHistoryFileLoader::Load(file.Path.string());

    REQUIRE_FALSE(history.HasStoredTimeDerivative());
    REQUIRE(history.Phi->At(0, 0) == Approx(1.25));
    REQUIRE(history.Phi->At(1, 1) == Approx(5.0));
}

TEST_CASE("OSCB loader rejects malformed payloads and channel counts", "[OSCB][History]") {
    SECTION("truncated payload") {
        FTemporaryOscbFile file("truncated");
        const std::string header =
            R"(# {"lines_contain_timestamp": True, "outresT": 1, "outresX": 2, "data_type": "fp32", "data_channels": 1, "t": 1.0, "L": 2.0})";
        WriteOscb(file.Path, header, std::vector<float>{0.0F, 1.0F});

        REQUIRE_THROWS(Modes::FHistoryFileLoader::Load(file.Path.string()));
    }

    SECTION("unsupported channels") {
        FTemporaryOscbFile file("channels");
        const std::string header =
            R"(# {"lines_contain_timestamp": True, "outresT": 1, "outresX": 1, "data_type": "fp32", "data_channels": 3, "t": 1.0, "L": 2.0})";
        WriteOscb(file.Path, header, std::vector<float>{});

        REQUIRE_THROWS(Modes::FHistoryFileLoader::Load(file.Path.string()));
    }
}
