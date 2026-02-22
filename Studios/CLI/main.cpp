#include "CrashPad.h"

#include "Core/Controller/Parameter/BuiltinParameters.h"

#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include "Models/KleinGordon/RtoR-Montecarlo/V2/RtoR-Hamiltonian-MetropolisHastings-RecipeV2.h"
#include "Models/Stochastic-Path-Integral/SPINumericConfig.h"
#include "Models/Stochastic-Path-Integral/V2/SPI-RecipeV2.h"

#include <chrono>
#include <cstddef>
#include <exception>
#include <iostream>
#include <map>
#include <set>
#include <thread>

namespace {

    using namespace Slab;

    struct FParsedCommandLine {
        Vector<Str> Positionals;
        std::map<Str, Str> Options;
    };

    auto PrintUsage() -> void {
        std::cout
                << "Studios - CLI launcher for selected simulations\n\n"
                << "Usage:\n"
                << "  Studios list\n"
                << "  Studios run <simulation> [options]\n"
                << "  Studios <simulation> [options]\n\n"
                << "Available simulations:\n"
                << "  metropolis-v2   (alias: v2-metropolis)\n"
                << "  spi-v2          (alias: v2-spi)\n\n"
                << "Common options:\n"
                << "  --batch <n>     Max integration batch size (default: 2048)\n"
                << "  --interval <n>  Listener trigger interval in steps\n\n"
                << "metropolis-v2 options:\n"
                << "  --steps <n>     Total Monte Carlo steps (default: 1000)\n\n"
                << "spi-v2 options:\n"
                << "  --steps <n>     Stochastic integration steps (default: 20)\n"
                << "  --dt <x>        Stochastic time step (default: 0.05)\n"
                << "  --time <x>      Spatial-time extent for SPI grid (default: 1.0)\n"
                << "  --L <x>         Spatial length (default: 1.0)\n"
                << "  --N <n>         Spatial site count (default: 64)\n\n"
                << "Examples:\n"
                << "  Studios run metropolis-v2 --steps 5000 --interval 500\n"
                << "  Studios spi-v2 --steps 8 --dt 0.125 --time 0.5 --N 16 --interval 2\n";
    }

    auto PrintList() -> void {
        std::cout << "metropolis-v2\nspi-v2\n";
    }

    auto ParseCommandLine(const int argc, const char **argv) -> FParsedCommandLine {
        FParsedCommandLine parsed;

        for (int i = 1; i < argc; ++i) {
            const Str arg = argv[i];

            if (arg.rfind("--", 0) == 0) {
                if (arg == "--help") {
                    parsed.Options["help"] = "true";
                    continue;
                }

                if (i + 1 >= argc) {
                    throw Exception("Missing value for option '" + arg + "'.");
                }

                parsed.Options[arg.substr(2)] = argv[++i];
                continue;
            }

            parsed.Positionals.push_back(arg);
        }

        return parsed;
    }

    auto ParseUIntBigOption(const FParsedCommandLine &cmd,
                            const Str &name,
                            const UIntBig defaultValue) -> UIntBig {
        const auto it = cmd.Options.find(name);
        if (it == cmd.Options.end()) return defaultValue;

        try {
            return static_cast<UIntBig>(std::stoull(it->second));
        } catch (const std::exception &) {
            throw Exception("Invalid unsigned integer for '--" + name + "': '" + it->second + "'.");
        }
    }

    auto ParseUIntOption(const FParsedCommandLine &cmd,
                         const Str &name,
                         const UInt defaultValue) -> UInt {
        return static_cast<UInt>(ParseUIntBigOption(cmd, name, defaultValue));
    }

    auto ParseFloatOption(const FParsedCommandLine &cmd,
                          const Str &name,
                          const DevFloat defaultValue) -> DevFloat {
        const auto it = cmd.Options.find(name);
        if (it == cmd.Options.end()) return defaultValue;

        try {
            return static_cast<DevFloat>(std::stod(it->second));
        } catch (const std::exception &) {
            throw Exception("Invalid numeric value for '--" + name + "': '" + it->second + "'.");
        }
    }

    auto ValidateOptions(const FParsedCommandLine &cmd, const std::set<Str> &allowed) -> void {
        for (const auto &[key, _] : cmd.Options) {
            if (key == "help") continue;
            if (allowed.contains(key)) continue;
            throw Exception("Unknown option '--" + key + "'.");
        }
    }

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

    auto PrintTaskSummary(const Math::Numerics::V2::FNumericTaskV2 &task) -> void {
        const auto cursor = task.GetCursor();
        std::cout << '\n' << "Finished at step=" << cursor.Step;
        if (cursor.SimulationTime.has_value()) {
            std::cout << " t=" << *cursor.SimulationTime;
        }
        std::cout << '\n';
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

    auto RunMetropolisV2(const FParsedCommandLine &cmd) -> int {
        using namespace Slab::Math::Numerics::V2;
        using namespace Slab::Models::KGRtoR::Metropolis::V2;

        ValidateOptions(cmd, {"steps", "interval", "batch"});

        const auto maxSteps = ParseUIntBigOption(cmd, "steps", 1000);
        const auto interval = ParseUIntBigOption(cmd, "interval", 1000);
        const auto batch = ParseUIntBigOption(cmd, "batch", 2048);

        auto recipe = New<FRtoRHamiltonianMetropolisHastingsRecipeV2>(maxSteps, interval);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(batch));

        const auto status = RunTaskAndWait(*task);
        PrintTaskSummary(*task);
        return ExitCodeFromTaskStatus(status);
    }

    auto ConfigureSPINumericConfig(const TPointer<Slab::Models::StochasticPathIntegrals::SPINumericConfig> &config,
                                   const FParsedCommandLine &cmd) -> void {
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

        pL->SetValue(ParseFloatOption(cmd, "L", 1.0));
        pT->SetValue(ParseFloatOption(cmd, "time", 1.0));
        pN->SetValue(static_cast<int>(ParseUIntOption(cmd, "N", 64)));
        pDT->SetValue(ParseFloatOption(cmd, "dt", 0.05));
        pNT->SetValue(static_cast<int>(ParseUIntOption(cmd, "steps", 20)));
    }

    auto RunSPIV2(const FParsedCommandLine &cmd) -> int {
        using namespace Slab::Math::Numerics::V2;
        using namespace Slab::Models::StochasticPathIntegrals;
        using namespace Slab::Models::StochasticPathIntegrals::V2;

        ValidateOptions(cmd, {"steps", "interval", "batch", "L", "time", "N", "dt"});

        const auto interval = ParseUIntBigOption(cmd, "interval", 10);
        const auto batch = ParseUIntBigOption(cmd, "batch", 2048);

        auto numericConfig = New<SPINumericConfig>();
        ConfigureSPINumericConfig(numericConfig, cmd);

        auto recipe = New<FSPIRecipeV2>(numericConfig, interval);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(batch));

        const auto status = RunTaskAndWait(*task);
        PrintTaskSummary(*task);
        return ExitCodeFromTaskStatus(status);
    }

    auto DispatchCommand(const FParsedCommandLine &cmd) -> int {
        if (cmd.Options.contains("help") || cmd.Positionals.empty()) {
            PrintUsage();
            return 0;
        }

        if (cmd.Positionals[0] == "list") {
            PrintList();
            return 0;
        }

        size_t simIndex = 0;
        if (cmd.Positionals[0] == "run") {
            if (cmd.Positionals.size() < 2) {
                throw Exception("Missing simulation name after 'run'.");
            }
            simIndex = 1;
        }

        if (cmd.Positionals.size() > simIndex + 1) {
            throw Exception("Unexpected positional argument '" + cmd.Positionals[simIndex + 1] + "'.");
        }

        const auto &simName = cmd.Positionals[simIndex];

        if (simName == "metropolis-v2" || simName == "v2-metropolis") {
            return RunMetropolisV2(cmd);
        }

        if (simName == "spi-v2" || simName == "v2-spi") {
            return RunSPIV2(cmd);
        }

        throw Exception("Unknown simulation '" + simName + "'. Use 'Studios list'.");
    }

    auto RunMain(const int argc, const char **argv) -> int {
        const auto cmd = ParseCommandLine(argc, argv);
        return DispatchCommand(cmd);
    }

} // namespace

auto main(const int argc, const char **argv) -> int {
    return Slab::SafetyNet::jump(RunMain, argc, argv);
}
