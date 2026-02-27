#include "CrashPad.h"

#include "Core/Controller/CommandLine/CommandLineParserDefs.h"
#include "Core/SlabCore.h"

#include "StudioSlab.h"
#include "../../Slab/Studios/Common/Simulations/V2/MetropolisSliceV2.h"
#include "../../Slab/Studios/Common/Simulations/V2/KGR2toRBaselineSliceV2.h"
#include "../../Slab/Studios/Common/Simulations/V2/KGRtoRPlaneWavesSliceV2.h"
#include "../../Slab/Studios/Common/Simulations/V2/MolecularDynamicsSliceV2.h"
#include "../../Slab/Studios/Common/Simulations/V2/SPISliceV2.h"
#include "../../Slab/Studios/Common/Simulations/V2/XYSliceV2.h"

#include <algorithm>
#include <cctype>
#include <iostream>

namespace {

    using namespace Slab;
    namespace StudiosSimV2 = Slab::Studios::Common::Simulations::V2;
    using StudiosSimV2::FMetropolisExecutionConfigV2;
    using StudiosSimV2::FR2toRBaselineExecutionConfig;
    using StudiosSimV2::FRtoRPlaneWavesExecutionConfig;
    using StudiosSimV2::FMolecularDynamicsExecutionConfigV2;
    using StudiosSimV2::FSPIExecutionConfig;
    using StudiosSimV2::FXYExecutionConfigV2;

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
                << "  kg2d         Run V2 KGR2toR baseline slice\n\n"
                << "  moldyn       Run V2 Molecular Dynamics baseline slice\n\n"
                << "  xy           Run V2 XY Metropolis lattice slice\n\n"
                << "Aliases:\n"
                << "  metropolis-v2, v2-metropolis\n"
                << "  spi-v2, v2-spi\n"
                << "  rtor-v2, rtor-plane-waves, v2-rtor\n\n"
                << "  kg2d-v2, r2tor-v2, v2-kg2d\n\n"
                << "  md-v2, molecular-dynamics, v2-moldyn\n\n"
                << "  xy-v2, v2-xy\n\n"
                << "Examples:\n"
                << "  Studios metropolis --steps 5000 --interval 500\n"
                << "  Studios metropolis --gl --steps 5000 --interval 500 --monitor-interval 100\n"
                << "  Studios spi --steps 8 --dt 0.125 --time 0.5 --N 16 --interval 2\n"
                << "  Studios spi --gl --steps 2000 --interval 50 --monitor-interval 2\n"
                << "  Studios rtor --steps 500 --dt 0.01 --L 10 --N 256 --Q 1 --harmonic 2\n"
                << "  Studios rtor --gl --steps 2000 --interval 50 --monitor-interval 2\n"
                << "  Studios kg2d --steps 300 --L 12 --N 128 --rdt 0.1 --pulse-width 0.35\n"
                << "  Studios kg2d --gl --steps 500 --interval 20 --monitor-interval 2\n"
                << "  Studios kg2d --gl --monitor-control-source --steps 500 --forcing-enabled --forcing-amplitude 0.2\n"
                << "  Studios kg2d --steps 500 --forcing-enabled --forcing-amplitude 0.2 --forcing-width 0.25\n"
                << "  Studios moldyn --steps 400 --time 20 --N 256 --L 50 --model softdisk\n"
                << "  Studios moldyn --gl --steps 400 --time 20 --N 256 --L 50 --interval 20 --monitor-interval 5\n"
                << "  Studios xy --steps 2000 --L 64 --temperature 0.7\n"
                << "  Studios xy --gl --steps 4000 --L 128 --temperature 0.75 --monitor-interval 10\n"
                << "  Studios spi --help\n";
    }

    auto PrintList() -> void {
        std::cout << "metropolis\nspi\nrtor\nkg2d\nmoldyn\nxy\n";
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
        CLOptionsDescription options("Studios metropolis", "Run the native V2 Metropolis RtoR slice.");
        options.add_options()
            ("h,help", "Show this help")
            ("gl", "Run with passive OpenGL monitor (real app loop)")
            ("steps", "Total Monte Carlo steps", cxxopts::value<UIntBig>()->default_value("1000"))
            ("interval", "Output/listener interval (steps)", cxxopts::value<UIntBig>()->default_value("1000"))
            ("monitor-interval",
             "Snapshot/monitor interval (steps) for --gl; defaults to --interval",
             cxxopts::value<UIntBig>())
            ("batch", "Max integration batch size", cxxopts::value<UIntBig>()->default_value("2048"));

        const auto result = ParseSubcommandOptions(argc, argv, options);
        if (result.count("help") > 0) {
            std::cout << options.help() << '\n';
            return 0;
        }

        FMetropolisExecutionConfigV2 cfg;
        cfg.Steps = result["steps"].as<UIntBig>();
        cfg.Interval = result["interval"].as<UIntBig>();
        cfg.MonitorInterval = result.count("monitor-interval") > 0
            ? result["monitor-interval"].as<UIntBig>()
            : cfg.Interval;
        cfg.Batch = result["batch"].as<UIntBig>();
        cfg.bEnableGLMonitor = result.count("gl") > 0;
        return StudiosSimV2::RunMetropolisV2(cfg);
    }

    auto RunSPICommand(const int argc, const char **argv) -> int {
        using namespace Slab::Math::Numerics::V2;

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

        return StudiosSimV2::RunSPIV2(cfg);
    }

    auto RunRtoRCommand(const int argc, const char **argv) -> int {
        using namespace Slab::Math::Numerics::V2;

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
            ("history-summary", "Attach V2 cursor history listener and print a run summary")
            ("snapshot-summary", "Attach V2 final snapshot listener and print snapshot summary")
            ("dft-probe-index",
             "Attach V2 scalar time DFT listener sampling phi[index] (headless or --gl)",
             cxxopts::value<UInt>())
            ("dft-interval",
             "DFT sampling interval (steps); defaults to --interval",
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
        cfg.DFTInterval = result.count("dft-interval") > 0
            ? result["dft-interval"].as<UIntBig>()
            : UIntBig(0);
        cfg.Batch = result["batch"].as<UIntBig>();
        cfg.bEnableGLMonitor = result.count("gl") > 0;
        cfg.bHistorySummary = result.count("history-summary") > 0;
        cfg.bSnapshotSummary = result.count("snapshot-summary") > 0;

        if (result.count("dt") > 0) {
            cfg.Dt = result["dt"].as<DevFloat>();
        }
        if (result.count("dft-probe-index") > 0) {
            cfg.DFTProbeIndex = result["dft-probe-index"].as<UInt>();
        }
        return StudiosSimV2::RunRtoRPlaneWavesV2(cfg);
    }

    auto RunKG2DCommand(const int argc, const char **argv) -> int {
        CLOptionsDescription options("Studios kg2d", "Run the native V2 KGR2toR baseline slice.");
        options.add_options()
            ("h,help", "Show this help")
            ("gl", "Run with passive OpenGL monitor (real app loop)")
            ("steps", "Integration steps", cxxopts::value<UIntBig>()->default_value("200"))
            ("L", "Spatial length", cxxopts::value<DevFloat>()->default_value("12.0"))
            ("N", "Grid side size", cxxopts::value<UInt>()->default_value("128"))
            ("rdt", "Legacy CFL ratio r_dt with dt = r_dt*(L/N)", cxxopts::value<DevFloat>()->default_value("0.1"))
            ("x-center", "Pulse center x", cxxopts::value<DevFloat>()->default_value("0.0"))
            ("y-center", "Pulse center y", cxxopts::value<DevFloat>()->default_value("0.0"))
            ("pulse-width", "Regularized delta width", cxxopts::value<DevFloat>()->default_value("0.35"))
            ("phi-amplitude", "Initial phi pulse amplitude", cxxopts::value<DevFloat>()->default_value("0.0"))
            ("dphi-dt-amplitude", "Initial dphi/dt pulse amplitude", cxxopts::value<DevFloat>()->default_value("1.0"))
            ("forcing-enabled", "Enable external forcing source during integration")
            ("forcing-amplitude", "External forcing amplitude", cxxopts::value<DevFloat>()->default_value("0.0"))
            ("forcing-width", "External forcing width", cxxopts::value<DevFloat>()->default_value("0.35"))
            ("forcing-x-center", "External forcing center x", cxxopts::value<DevFloat>()->default_value("0.0"))
            ("forcing-y-center", "External forcing center y", cxxopts::value<DevFloat>()->default_value("0.0"))
            ("live-control-forcing", "Enable LiveControl forcing binding (topic-driven forcing values)")
            ("control-topic-prefix",
             "LiveControl topic prefix (e.g. labv2/control/kg2d)",
             cxxopts::value<Str>()->default_value("labv2/control/kg2d"))
            ("control-sample-interval",
             "LiveControl sample interval (steps)",
             cxxopts::value<UIntBig>()->default_value("1"))
            ("monitor-control-source",
             "For --gl, open in-monitor control source UI and publish forcing values to LiveControl")
            ("interval", "Console/listener interval (steps)", cxxopts::value<UIntBig>()->default_value("20"))
            ("monitor-interval",
             "Live-view publish interval (steps) for --gl; defaults to --interval",
             cxxopts::value<UIntBig>())
            ("batch", "Max integration batch size", cxxopts::value<UIntBig>()->default_value("2048"));

        const auto result = ParseSubcommandOptions(argc, argv, options);
        if (result.count("help") > 0) {
            std::cout << options.help() << '\n';
            return 0;
        }

        FR2toRBaselineExecutionConfig cfg;
        cfg.Steps = result["steps"].as<UIntBig>();
        cfg.L = result["L"].as<DevFloat>();
        cfg.N = result["N"].as<UInt>();
        cfg.RDt = result["rdt"].as<DevFloat>();
        cfg.XCenter = result["x-center"].as<DevFloat>();
        cfg.YCenter = result["y-center"].as<DevFloat>();
        cfg.PulseWidth = result["pulse-width"].as<DevFloat>();
        cfg.PhiAmplitude = result["phi-amplitude"].as<DevFloat>();
        cfg.DPhiDtAmplitude = result["dphi-dt-amplitude"].as<DevFloat>();
        cfg.ForcingAmplitude = result["forcing-amplitude"].as<DevFloat>();
        cfg.ForcingWidth = result["forcing-width"].as<DevFloat>();
        cfg.ForcingXCenter = result["forcing-x-center"].as<DevFloat>();
        cfg.ForcingYCenter = result["forcing-y-center"].as<DevFloat>();
        cfg.ControlTopicPrefix = result["control-topic-prefix"].as<Str>();
        cfg.ControlSampleInterval = result["control-sample-interval"].as<UIntBig>();
        cfg.bEnableMonitorControlPublisher = result.count("monitor-control-source") > 0;
        cfg.bEnableLiveControlForcing = result.count("live-control-forcing") > 0 || cfg.bEnableMonitorControlPublisher;
        cfg.bForcingEnabled = result.count("forcing-enabled") > 0 ||
            !Slab::Common::AreEqual(cfg.ForcingAmplitude, 0.0);
        cfg.Interval = result["interval"].as<UIntBig>();
        cfg.MonitorInterval = result.count("monitor-interval") > 0
            ? result["monitor-interval"].as<UIntBig>()
            : cfg.Interval;
        cfg.Batch = result["batch"].as<UIntBig>();
        cfg.bEnableGLMonitor = result.count("gl") > 0;
        if (cfg.bEnableMonitorControlPublisher && !cfg.bEnableGLMonitor) {
            throw Exception("--monitor-control-source requires --gl.");
        }

        return StudiosSimV2::RunR2toRBaselineV2(cfg);
    }

    auto NormalizeMDModelName(const Str &raw) -> Str {
        Str lowered = raw;
        std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](const unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return lowered;
    }

    auto ParseMDModel(const Str &raw) -> StudiosSimV2::EMDInteractionModelV2 {
        const auto value = NormalizeMDModelName(raw);
        if (value == "softdisk" || value == "soft-disk" || value == "soft_disk") {
            return StudiosSimV2::EMDInteractionModelV2::SoftDisk;
        }
        if (value == "lennardjones" || value == "lennard-jones" || value == "lj") {
            return StudiosSimV2::EMDInteractionModelV2::LennardJones;
        }
        throw Exception("Unknown moldyn model '" + raw + "'. Expected softdisk or lennard-jones.");
    }

    auto RunMolecularDynamicsCommand(const int argc, const char **argv) -> int {
        CLOptionsDescription options("Studios moldyn", "Run the native V2 Molecular Dynamics baseline slice.");
        options.add_options()
            ("h,help", "Show this help")
            ("gl", "Run with passive OpenGL monitor (real app loop)")
            ("steps", "Integration steps", cxxopts::value<UIntBig>()->default_value("1000"))
            ("time", "Total simulation time", cxxopts::value<DevFloat>()->default_value("50.0"))
            ("L", "Spatial box length", cxxopts::value<DevFloat>()->default_value("50.0"))
            ("N", "Particle count (N >= 128)", cxxopts::value<UInt>()->default_value("256"))
            ("temperature", "Langevin temperature parameter", cxxopts::value<DevFloat>()->default_value("0.0"))
            ("dissipation", "Dissipation coefficient", cxxopts::value<DevFloat>()->default_value("0.0"))
            ("model", "Interaction model: softdisk | lennard-jones", cxxopts::value<Str>()->default_value("softdisk"))
            ("interval", "Console/listener interval (steps)", cxxopts::value<UIntBig>()->default_value("100"))
            ("monitor-interval",
             "Live-view publish interval; defaults to --interval",
             cxxopts::value<UIntBig>())
            ("batch", "Max integration batch size", cxxopts::value<UIntBig>()->default_value("512"));

        const auto result = ParseSubcommandOptions(argc, argv, options);
        if (result.count("help") > 0) {
            std::cout << options.help() << '\n';
            return 0;
        }

        FMolecularDynamicsExecutionConfigV2 cfg;
        cfg.Steps = result["steps"].as<UIntBig>();
        cfg.TotalTime = result["time"].as<DevFloat>();
        cfg.L = result["L"].as<DevFloat>();
        cfg.N = result["N"].as<UInt>();
        cfg.Temperature = result["temperature"].as<DevFloat>();
        cfg.Dissipation = result["dissipation"].as<DevFloat>();
        cfg.InteractionModel = ParseMDModel(result["model"].as<Str>());
        cfg.Interval = result["interval"].as<UIntBig>();
        cfg.MonitorInterval = result.count("monitor-interval") > 0
            ? result["monitor-interval"].as<UIntBig>()
            : cfg.Interval;
        cfg.Batch = result["batch"].as<UIntBig>();
        cfg.bEnableGLMonitor = result.count("gl") > 0;

        return StudiosSimV2::RunMolecularDynamicsV2(cfg);
    }

    auto RunXYCommand(const int argc, const char **argv) -> int {
        CLOptionsDescription options("Studios xy", "Run the native V2 XY Metropolis lattice slice.");
        options.add_options()
            ("h,help", "Show this help")
            ("gl", "Run with passive OpenGL monitor (real app loop)")
            ("steps", "Monte Carlo sweeps", cxxopts::value<UIntBig>()->default_value("2000"))
            ("L", "Lattice side length", cxxopts::value<UInt>()->default_value("64"))
            ("temperature", "Thermal bath temperature", cxxopts::value<DevFloat>()->default_value("0.7"))
            ("h-field", "External field coefficient", cxxopts::value<DevFloat>()->default_value("0.0"))
            ("delta-theta", "Proposal angle range", cxxopts::value<DevFloat>()->default_value("6.283185307179586"))
            ("ferromagnetic", "Start from ferromagnetic initial condition")
            ("interval", "Console/listener interval (steps)", cxxopts::value<UIntBig>()->default_value("100"))
            ("monitor-interval",
             "Live-view publish interval; defaults to --interval",
             cxxopts::value<UIntBig>())
            ("batch", "Max integration batch size", cxxopts::value<UIntBig>()->default_value("1024"));

        const auto result = ParseSubcommandOptions(argc, argv, options);
        if (result.count("help") > 0) {
            std::cout << options.help() << '\n';
            return 0;
        }

        FXYExecutionConfigV2 cfg;
        cfg.Steps = result["steps"].as<UIntBig>();
        cfg.L = result["L"].as<UInt>();
        cfg.Temperature = result["temperature"].as<DevFloat>();
        cfg.ExternalField = result["h-field"].as<DevFloat>();
        cfg.DeltaTheta = result["delta-theta"].as<DevFloat>();
        cfg.bFerromagneticInitial = result.count("ferromagnetic") > 0;
        cfg.Interval = result["interval"].as<UIntBig>();
        cfg.MonitorInterval = result.count("monitor-interval") > 0
            ? result["monitor-interval"].as<UIntBig>()
            : cfg.Interval;
        cfg.Batch = result["batch"].as<UIntBig>();
        cfg.bEnableGLMonitor = result.count("gl") > 0;

        return StudiosSimV2::RunXYV2(cfg);
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

    auto IsKG2DCommand(const Str &name) -> bool {
        return name == "kg2d" || name == "kg2d-v2" || name == "r2tor-v2" || name == "v2-kg2d";
    }

    auto IsMolecularDynamicsCommand(const Str &name) -> bool {
        return name == "moldyn" || name == "md-v2" || name == "molecular-dynamics" || name == "v2-moldyn";
    }

    auto IsXYCommand(const Str &name) -> bool {
        return name == "xy" || name == "xy-v2" || name == "v2-xy";
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
            if (IsKG2DCommand(sub)) return RunKG2DCommand(argc - 2, argv + 2);
            if (IsMolecularDynamicsCommand(sub)) return RunMolecularDynamicsCommand(argc - 2, argv + 2);
            if (IsXYCommand(sub)) return RunXYCommand(argc - 2, argv + 2);

            throw Exception("Unknown subprogram '" + sub + "'. Use 'Studios list'.");
        }

        if (IsMetropolisCommand(first)) return RunMetropolisCommand(argc - 1, argv + 1);
        if (IsSPICommand(first)) return RunSPICommand(argc - 1, argv + 1);
        if (IsRtoRCommand(first)) return RunRtoRCommand(argc - 1, argv + 1);
        if (IsKG2DCommand(first)) return RunKG2DCommand(argc - 1, argv + 1);
        if (IsMolecularDynamicsCommand(first)) return RunMolecularDynamicsCommand(argc - 1, argv + 1);
        if (IsXYCommand(first)) return RunXYCommand(argc - 1, argv + 1);

        throw Exception("Unknown subprogram '" + first + "'. Use 'Studios list'.");
    }

    auto RunMain(const int argc, const char **argv) -> int {
        return DispatchRoot(argc, argv);
    }

} // namespace

auto main(const int argc, const char **argv) -> int {
    return Slab::SafetyNet::jump(RunMain, argc, argv);
}
