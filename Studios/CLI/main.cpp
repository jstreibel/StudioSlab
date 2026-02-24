#include "CrashPad.h"

#include "Core/Controller/CommandLine/CommandLineParserDefs.h"
#include "Core/Controller/Parameter/BuiltinParameters.h"
#include "Core/SlabCore.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include "Models/KleinGordon/RtoR/LinearStepping/V2/KG-RtoR-PlaneWaves-RecipeV2.h"
#include "Models/KleinGordon/RtoR-Montecarlo/V2/RtoR-Hamiltonian-MetropolisHastings-RecipeV2.h"
#include "Models/Stochastic-Path-Integral/SPINumericConfig.h"
#include "Models/Stochastic-Path-Integral/V2/SPI-RecipeV2.h"

#include "StudioSlab.h"
#include "../Common/Monitors/V2/KGRtoRPlaneWavesPassiveMonitorWindowV2.h"
#include "../Common/Monitors/V2/SPIPassiveMonitorWindowV2.h"
#include "../Common/NumericsV2TaskUtils.h"
#include "../Common/V2SimulationRunners.h"

#include <iostream>

namespace {

    using namespace Slab;

    struct FSPIExecutionConfig;
    struct FRtoRPlaneWavesExecutionConfig;

    auto ConfigureSPINumericConfig(const TPointer<Slab::Models::StochasticPathIntegrals::SPINumericConfig> &config,
                                   DevFloat L,
                                   DevFloat Time,
                                   UInt N,
                                   DevFloat Dt,
                                   UInt Steps) -> void;
    auto BuildRtoRPlaneWavesRecipeConfig(const FRtoRPlaneWavesExecutionConfig &cfg)
        -> Slab::Models::KGRtoR::PlaneWaves::V2::FKGRtoRPlaneWavesConfigV2;

    struct FSPIExecutionConfig {
        UInt Steps = 20;
        DevFloat Dt = 0.05;
        DevFloat Time = 1.0;
        DevFloat L = 1.0;
        UInt N = 64;
        UIntBig Interval = 10;
        UIntBig MonitorInterval = 10;
        UIntBig Batch = 2048;
        bool bEnableGLMonitor = false;
    };

    struct FRtoRPlaneWavesExecutionConfig {
        UIntBig Steps = 200;
        DevFloat Dt = -1.0;
        DevFloat L = 10.0;
        UInt N = 256;
        DevFloat XCenter = 0.0;
        DevFloat Q = 1.0;
        UInt Harmonic = 2;
        UIntBig Interval = 20;
        UIntBig MonitorInterval = 20;
        UIntBig Batch = 2048;
        bool bEnableGLMonitor = false;
    };

    auto RunTaskWithPassiveSPIGLMonitor(const FSPIExecutionConfig &cfg) -> int {
        using namespace Slab::Models::StochasticPathIntegrals;
        using namespace Slab::Models::StochasticPathIntegrals::V2;

        auto liveView = New<Math::LiveData::V2::FSessionLiveViewV2>();

        auto numericConfig = New<SPINumericConfig>();
        ConfigureSPINumericConfig(numericConfig, cfg.L, cfg.Time, cfg.N, cfg.Dt, cfg.Steps);

        auto recipe = New<FSPIRecipeV2>(numericConfig, cfg.Interval, liveView);
        recipe->SetLiveViewIntervalSteps(cfg.MonitorInterval);
        auto monitor = New<Slab::Studios::Common::Monitors::V2::FSPIPassiveMonitorWindowV2>(
            liveView, static_cast<UIntBig>(cfg.Steps));
        return Slab::Studios::Common::RunGLFWMonitoredNumericTaskV2(
            "Studios SPI V2 Monitor", recipe, monitor, static_cast<size_t>(cfg.Batch));
    }

    auto RunTaskWithPassiveRtoRGLMonitor(const FRtoRPlaneWavesExecutionConfig &cfg) -> int {
        using namespace Slab::Models::KGRtoR::PlaneWaves::V2;

        auto liveView = New<Math::LiveData::V2::FSessionLiveViewV2>();
        auto recipe = New<FKGRtoRPlaneWavesRecipeV2>(BuildRtoRPlaneWavesRecipeConfig(cfg), cfg.Interval, liveView);
        recipe->SetLiveViewIntervalSteps(cfg.MonitorInterval);

        auto monitor = New<Slab::Studios::Common::Monitors::V2::FRtoRPlaneWavesPassiveMonitorWindowV2>(
            liveView, cfg.Steps);
        return Slab::Studios::Common::RunGLFWMonitoredNumericTaskV2(
            "Studios KGRtoR Plane Waves V2 Monitor", recipe, monitor, static_cast<size_t>(cfg.Batch));
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
                << "  spi          Run V2 SPI ODE/time-aware slice\n"
                << "  rtor         Run V2 KGRtoR plane-waves slice\n\n"
                << "Aliases:\n"
                << "  metropolis-v2, v2-metropolis\n"
                << "  spi-v2, v2-spi\n"
                << "  rtor-v2, rtor-plane-waves, v2-rtor\n\n"
                << "Examples:\n"
                << "  Studios metropolis --steps 5000 --interval 500\n"
                << "  Studios spi --steps 8 --dt 0.125 --time 0.5 --N 16 --interval 2\n"
                << "  Studios spi --gl --steps 2000 --interval 50 --monitor-interval 2\n"
                << "  Studios rtor --steps 500 --dt 0.01 --L 10 --N 256 --Q 1 --harmonic 2\n"
                << "  Studios rtor --gl --steps 2000 --interval 50 --monitor-interval 2\n"
                << "  Studios spi --help\n";
    }

    auto PrintList() -> void {
        std::cout << "metropolis\nspi\nrtor\n";
    }

    auto NormalizeShortLongSingleLetterOption(const Str &arg) -> Str {
        // cxxopts naturally handles `-N`, while existing local habits often use `--N`.
        if (arg == "--N") return "-N";
        if (arg == "--L") return "-L";
        if (arg == "--Q") return "-Q";
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

        const auto status = Slab::Studios::Common::RunTaskAndWait(*task);
        Slab::Studios::Common::PrintNumericTaskSummary(*task);
        return Slab::Studios::Common::ExitCodeFromTaskStatus(status);
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
            ("gl", "Run with passive OpenGL monitor (real app loop)")
            ("steps", "Stochastic integration steps", cxxopts::value<UInt>()->default_value("20"))
            ("dt", "Stochastic timestep", cxxopts::value<DevFloat>()->default_value("0.05"))
            ("time", "Physical time extent", cxxopts::value<DevFloat>()->default_value("1.0"))
            ("L", "Spatial length", cxxopts::value<DevFloat>()->default_value("1.0"))
            ("N", "Spatial site count", cxxopts::value<UInt>()->default_value("64"))
            ("interval", "Output/listener interval (steps)", cxxopts::value<UIntBig>()->default_value("10"))
            ("monitor-interval",
             "Live-view publish interval (steps) for --gl; defaults to --interval",
             cxxopts::value<UIntBig>())
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
        const auto monitorInterval = result.count("monitor-interval") > 0
            ? result["monitor-interval"].as<UIntBig>()
            : interval;
        const auto batch = result["batch"].as<UIntBig>();
        const auto useGL = result.count("gl") > 0;

        FSPIExecutionConfig cfg;
        cfg.Steps = steps;
        cfg.Dt = dt;
        cfg.Time = time;
        cfg.L = L;
        cfg.N = N;
        cfg.Interval = interval;
        cfg.MonitorInterval = monitorInterval;
        cfg.Batch = batch;
        cfg.bEnableGLMonitor = useGL;

        if (cfg.bEnableGLMonitor) {
            return RunTaskWithPassiveSPIGLMonitor(cfg);
        }

        auto numericConfig = New<SPINumericConfig>();
        ConfigureSPINumericConfig(numericConfig, L, time, N, dt, steps);

        auto recipe = New<FSPIRecipeV2>(numericConfig, interval);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(batch));

        const auto status = Slab::Studios::Common::RunTaskAndWait(*task);
        Slab::Studios::Common::PrintNumericTaskSummary(*task);
        return Slab::Studios::Common::ExitCodeFromTaskStatus(status);
    }

    auto BuildRtoRPlaneWavesRecipeConfig(const FRtoRPlaneWavesExecutionConfig &cfg)
        -> Slab::Models::KGRtoR::PlaneWaves::V2::FKGRtoRPlaneWavesConfigV2 {
        Slab::Models::KGRtoR::PlaneWaves::V2::FKGRtoRPlaneWavesConfigV2 recipeCfg;
        recipeCfg.N = cfg.N;
        recipeCfg.L = cfg.L;
        recipeCfg.Dt = cfg.Dt;
        recipeCfg.Steps = cfg.Steps;
        recipeCfg.XCenter = cfg.XCenter;
        recipeCfg.Q = cfg.Q;
        recipeCfg.Harmonic = cfg.Harmonic;
        return recipeCfg;
    }

    auto RunRtoRCommand(const int argc, const char **argv) -> int {
        using namespace Slab::Math::Numerics::V2;
        using namespace Slab::Models::KGRtoR::PlaneWaves::V2;

        CLOptionsDescription options("Studios rtor", "Run the native V2 KGRtoR plane-waves slice.");
        options.add_options()
            ("h,help", "Show this help")
            ("gl", "Run with passive OpenGL monitor (real app loop)")
            ("steps", "Integration steps", cxxopts::value<UIntBig>()->default_value("200"))
            ("dt", "Timestep override; if omitted, uses legacy dt = 0.1*(L/N)", cxxopts::value<DevFloat>())
            ("L", "Spatial length", cxxopts::value<DevFloat>()->default_value("10.0"))
            ("N", "Spatial site count", cxxopts::value<UInt>()->default_value("256"))
            ("x-center", "Space center", cxxopts::value<DevFloat>()->default_value("0.0"))
            ("Q", "Plane-wave scale-invariant Q", cxxopts::value<DevFloat>()->default_value("1.0"))
            ("harmonic", "Plane-wave harmonic n (k=2*pi*n/L)", cxxopts::value<UInt>()->default_value("2"))
            ("interval", "Output/listener interval (steps)", cxxopts::value<UIntBig>()->default_value("20"))
            ("monitor-interval",
             "Live-view publish interval (steps) for --gl; defaults to --interval",
             cxxopts::value<UIntBig>())
            ("batch", "Max integration batch size", cxxopts::value<UIntBig>()->default_value("2048"));

        const auto result = ParseSubcommandOptions(argc, argv, options);
        if (result.count("help") > 0) {
            std::cout << options.help() << '\n';
            return 0;
        }

        FRtoRPlaneWavesExecutionConfig cfg;
        cfg.Steps = result["steps"].as<UIntBig>();
        cfg.L = result["L"].as<DevFloat>();
        cfg.N = result["N"].as<UInt>();
        cfg.XCenter = result["x-center"].as<DevFloat>();
        cfg.Q = result["Q"].as<DevFloat>();
        cfg.Harmonic = result["harmonic"].as<UInt>();
        cfg.Interval = result["interval"].as<UIntBig>();
        cfg.MonitorInterval = result.count("monitor-interval") > 0
            ? result["monitor-interval"].as<UIntBig>()
            : cfg.Interval;
        cfg.Batch = result["batch"].as<UIntBig>();
        cfg.bEnableGLMonitor = result.count("gl") > 0;

        if (cfg.N == 0) throw Exception("RtoR requires N > 0.");

        if (result.count("dt") > 0) {
            cfg.Dt = result["dt"].as<DevFloat>();
        } else {
            const auto h = cfg.L / static_cast<DevFloat>(cfg.N);
            cfg.Dt = static_cast<DevFloat>(0.1) * h; // Legacy KG default: dt/h = r = 0.1
        }

        if (cfg.bEnableGLMonitor) {
            return RunTaskWithPassiveRtoRGLMonitor(cfg);
        }

        auto recipe = New<FKGRtoRPlaneWavesRecipeV2>(BuildRtoRPlaneWavesRecipeConfig(cfg), cfg.Interval);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(cfg.Batch));

        const auto status = Slab::Studios::Common::RunTaskAndWait(*task);
        Slab::Studios::Common::PrintNumericTaskSummary(*task);
        return Slab::Studios::Common::ExitCodeFromTaskStatus(status);
    }

    auto IsMetropolisCommand(const Str &name) -> bool {
        return name == "metropolis" || name == "metropolis-v2" || name == "v2-metropolis";
    }

    auto IsSPICommand(const Str &name) -> bool {
        return name == "spi" || name == "spi-v2" || name == "v2-spi";
    }

    auto IsRtoRCommand(const Str &name) -> bool {
        return name == "rtor" || name == "rtor-v2" || name == "rtor-plane-waves" || name == "v2-rtor";
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
            if (IsRtoRCommand(sub)) return RunRtoRCommand(argc - 2, argv + 2);

            throw Exception("Unknown subprogram '" + sub + "'. Use 'Studios list'.");
        }

        if (IsMetropolisCommand(first)) return RunMetropolisCommand(argc - 1, argv + 1);
        if (IsSPICommand(first)) return RunSPICommand(argc - 1, argv + 1);
        if (IsRtoRCommand(first)) return RunRtoRCommand(argc - 1, argv + 1);

        throw Exception("Unknown subprogram '" + first + "'. Use 'Studios list'.");
    }

    auto RunMain(const int argc, const char **argv) -> int {
        return DispatchRoot(argc, argv);
    }

} // namespace

auto main(const int argc, const char **argv) -> int {
    return Slab::SafetyNet::jump(RunMain, argc, argv);
}
