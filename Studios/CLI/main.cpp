#include "CrashPad.h"

#include "Core/Controller/CommandLine/CommandLineParserDefs.h"
#include "Core/Controller/Parameter/BuiltinParameters.h"

#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include "Models/KleinGordon/RtoR-Montecarlo/V2/RtoR-Hamiltonian-MetropolisHastings-RecipeV2.h"
#include "Models/Stochastic-Path-Integral/SPINumericConfig.h"
#include "Models/Stochastic-Path-Integral/V2/SPI-RecipeV2.h"

#include <chrono>
#include <iostream>
#include <thread>

namespace {

    using namespace Slab;

    auto RunTaskAndWait(Slab::Core::FTask &task) -> Slab::Core::ETaskStatus {
        std::thread worker([&task] { task.Start(); });

        while (true) {
            const auto status = task.GetStatus();
            if (status != Slab::Core::TaskNotInitialized && status != Slab::Core::TaskRunning) {
                task.Release();
                worker.join();
                return status;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    auto ExitCodeFromTaskStatus(const Slab::Core::ETaskStatus status) -> int {
        switch (status) {
        case Slab::Core::TaskSuccess:
            return 0;
        case Slab::Core::TaskAborted:
            return 130;
        case Slab::Core::TaskError:
        case Slab::Core::TaskNotInitialized:
        case Slab::Core::TaskRunning:
            return 2;
        }

        return 2;
    }

    auto PrintTaskSummary(const Math::Numerics::V2::FNumericTaskV2 &task) -> void {
        const auto cursor = task.GetCursor();
        std::cout << '\n' << "Finished at step=" << cursor.Step;
        if (cursor.SimulationTime.has_value()) {
            std::cout << " t=" << *cursor.SimulationTime;
        }
        std::cout << '\n';
    }

    auto PrintRootUsage() -> void {
        std::cout
                << "Studios - CLI launcher for selected simulations\n\n"
                << "Usage:\n"
                << "  Studios list\n"
                << "  Studios <subprogram> [options]\n"
                << "  Studios run <subprogram> [options]\n\n"
                << "Subprograms:\n"
                << "  metropolis   Run V2 Hamiltonian RtoR Metropolis slice\n"
                << "  spi          Run V2 SPI ODE/time-aware slice\n\n"
                << "Aliases:\n"
                << "  metropolis-v2, v2-metropolis\n"
                << "  spi-v2, v2-spi\n\n"
                << "Examples:\n"
                << "  Studios metropolis --steps 5000 --interval 500\n"
                << "  Studios spi --steps 8 --dt 0.125 --time 0.5 --N 16 --interval 2\n"
                << "  Studios spi --help\n";
    }

    auto PrintList() -> void {
        std::cout << "metropolis\nspi\n";
    }

    auto NormalizeShortLongSingleLetterOption(const Str &arg) -> Str {
        // cxxopts naturally handles `-N`, while existing local habits often use `--N`.
        if (arg == "--N") return "-N";
        if (arg == "--L") return "-L";
        return arg;
    }

    auto BuildNormalizedArgvVector(const int argc, const char **argv) -> Vector<Str> {
        Vector<Str> normalized;
        normalized.reserve(argc);

        for (int i = 0; i < argc; ++i) {
            normalized.emplace_back(NormalizeShortLongSingleLetterOption(argv[i]));
        }

        return normalized;
    }

    auto BuildArgvPointers(Vector<Str> &argvStorage) -> Vector<const char *> {
        Vector<const char *> out;
        out.reserve(argvStorage.size());
        for (auto &arg : argvStorage) out.push_back(arg.c_str());
        return out;
    }

    auto ParseSubcommandOptions(const int argc,
                                const char **argv,
                                CLOptionsDescription &options) -> CLVariablesMap {
        auto normalizedArgvStorage = BuildNormalizedArgvVector(argc, argv);
        auto normalizedArgv = BuildArgvPointers(normalizedArgvStorage);

        return options.parse(static_cast<int>(normalizedArgv.size()), normalizedArgv.data());
    }

    auto RunMetropolisCommand(const int argc, const char **argv) -> int {
        using namespace Slab::Math::Numerics::V2;
        using namespace Slab::Models::KGRtoR::Metropolis::V2;

        CLOptionsDescription options("Studios metropolis", "Run the native V2 Metropolis RtoR slice.");
        options.add_options()
            ("h,help", "Show this help")
            ("steps", "Total Monte Carlo steps", cxxopts::value<UIntBig>()->default_value("1000"))
            ("interval", "Output/listener interval (steps)", cxxopts::value<UIntBig>()->default_value("1000"))
            ("batch", "Max integration batch size", cxxopts::value<UIntBig>()->default_value("2048"));

        const auto result = ParseSubcommandOptions(argc, argv, options);
        if (result.count("help") > 0) {
            std::cout << options.help() << '\n';
            return 0;
        }

        const auto steps = result["steps"].as<UIntBig>();
        const auto interval = result["interval"].as<UIntBig>();
        const auto batch = result["batch"].as<UIntBig>();

        auto recipe = New<FRtoRHamiltonianMetropolisHastingsRecipeV2>(steps, interval);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(batch));

        const auto status = RunTaskAndWait(*task);
        PrintTaskSummary(*task);
        return ExitCodeFromTaskStatus(status);
    }

    auto ConfigureSPINumericConfig(const TPointer<Slab::Models::StochasticPathIntegrals::SPINumericConfig> &config,
                                   const DevFloat L,
                                   const DevFloat Time,
                                   const UInt N,
                                   const DevFloat Dt,
                                   const UInt Steps) -> void {
        using namespace Slab::Core;

        const auto iface = config->GetInterface();
        if (iface == nullptr) throw Exception("SPI config interface is null.");

        auto pL = DynamicPointerCast<RealParameter>(iface->GetParameter("length"));
        auto pT = DynamicPointerCast<RealParameter>(iface->GetParameter("time"));
        auto pN = DynamicPointerCast<IntegerParameter>(iface->GetParameter("site_count"));
        auto pDT = DynamicPointerCast<RealParameter>(iface->GetParameter("dT"));
        auto pNT = DynamicPointerCast<IntegerParameter>(iface->GetParameter("stochastic_time_steps"));

        if (pL == nullptr || pT == nullptr || pN == nullptr || pDT == nullptr || pNT == nullptr) {
            throw Exception("SPI config parameters are incomplete.");
        }

        pL->SetValue(L);
        pT->SetValue(Time);
        pN->SetValue(static_cast<int>(N));
        pDT->SetValue(Dt);
        pNT->SetValue(static_cast<int>(Steps));
    }

    auto RunSPICommand(const int argc, const char **argv) -> int {
        using namespace Slab::Math::Numerics::V2;
        using namespace Slab::Models::StochasticPathIntegrals;
        using namespace Slab::Models::StochasticPathIntegrals::V2;

        CLOptionsDescription options("Studios spi", "Run the native V2 SPI slice.");
        options.add_options()
            ("h,help", "Show this help")
            ("steps", "Stochastic integration steps", cxxopts::value<UInt>()->default_value("20"))
            ("dt", "Stochastic timestep", cxxopts::value<DevFloat>()->default_value("0.05"))
            ("time", "Physical time extent", cxxopts::value<DevFloat>()->default_value("1.0"))
            ("L", "Spatial length", cxxopts::value<DevFloat>()->default_value("1.0"))
            ("N", "Spatial site count", cxxopts::value<UInt>()->default_value("64"))
            ("interval", "Output/listener interval (steps)", cxxopts::value<UIntBig>()->default_value("10"))
            ("batch", "Max integration batch size", cxxopts::value<UIntBig>()->default_value("2048"));

        const auto result = ParseSubcommandOptions(argc, argv, options);
        if (result.count("help") > 0) {
            std::cout << options.help() << '\n';
            return 0;
        }

        const auto steps = result["steps"].as<UInt>();
        const auto dt = result["dt"].as<DevFloat>();
        const auto time = result["time"].as<DevFloat>();
        const auto L = result["L"].as<DevFloat>();
        const auto N = result["N"].as<UInt>();
        const auto interval = result["interval"].as<UIntBig>();
        const auto batch = result["batch"].as<UIntBig>();

        auto numericConfig = New<SPINumericConfig>();
        ConfigureSPINumericConfig(numericConfig, L, time, N, dt, steps);

        auto recipe = New<FSPIRecipeV2>(numericConfig, interval);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(batch));

        const auto status = RunTaskAndWait(*task);
        PrintTaskSummary(*task);
        return ExitCodeFromTaskStatus(status);
    }

    auto IsMetropolisCommand(const Str &name) -> bool {
        return name == "metropolis" || name == "metropolis-v2" || name == "v2-metropolis";
    }

    auto IsSPICommand(const Str &name) -> bool {
        return name == "spi" || name == "spi-v2" || name == "v2-spi";
    }

    auto DispatchRoot(const int argc, const char **argv) -> int {
        if (argc <= 1) {
            PrintRootUsage();
            return 0;
        }

        const Str first = argv[1];

        if (first == "--help" || first == "-h" || first == "help") {
            PrintRootUsage();
            return 0;
        }

        if (first == "list") {
            if (argc > 2) throw Exception("Command 'list' does not take extra arguments.");
            PrintList();
            return 0;
        }

        if (first == "run") {
            if (argc <= 2) throw Exception("Missing subprogram after 'run'.");

            const Str sub = argv[2];
            if (IsMetropolisCommand(sub)) return RunMetropolisCommand(argc - 2, argv + 2);
            if (IsSPICommand(sub)) return RunSPICommand(argc - 2, argv + 2);

            throw Exception("Unknown subprogram '" + sub + "'. Use 'Studios list'.");
        }

        if (IsMetropolisCommand(first)) return RunMetropolisCommand(argc - 1, argv + 1);
        if (IsSPICommand(first)) return RunSPICommand(argc - 1, argv + 1);

        throw Exception("Unknown subprogram '" + first + "'. Use 'Studios list'.");
    }

    auto RunMain(const int argc, const char **argv) -> int {
        return DispatchRoot(argc, argv);
    }

} // namespace

auto main(const int argc, const char **argv) -> int {
    return Slab::SafetyNet::jump(RunMain, argc, argv);
}
