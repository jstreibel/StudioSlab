#include <catch2/catch_all.hpp>

#include "Core/Composition/V2/LegacyRuntimeBootstrapV2.h"
#include "Core/Composition/V2/Modules/LegacyBridgeModuleV2.h"
#include "Core/Composition/V2/RuntimeContextV2.h"
#include "Core/Composition/V2/Services/ReflectionServiceV2.h"
#include "Core/Composition/V2/Services/TaskServiceV2.h"
#include "Core/Platform/V2/LegacyPlatformHostV2.h"

namespace {

    using namespace Slab;
    namespace CompositionV2 = Slab::Core::Composition::V2;
    namespace PlatformV2 = Slab::Core::Platform::V2;

    class IFakeServiceV2 {
    public:
        virtual ~IFakeServiceV2() = default;
        [[nodiscard]] virtual auto GetValue() const -> int = 0;
    };

    class FFakeServiceV2 final : public IFakeServiceV2 {
    public:
        explicit FFakeServiceV2(const int value) : Value(value) {
        }

        [[nodiscard]] auto GetValue() const -> int override {
            return Value;
        }

    private:
        int Value = 0;
    };

    class FCountingModuleV2 final : public CompositionV2::IModuleV2 {
    public:
        FCountingModuleV2() {
            Descriptor.ModuleId = "counting";
            Descriptor.DisplayName = "Counting";
        }

        [[nodiscard]] auto GetDescriptor() const -> const CompositionV2::FModuleDescriptorV2 & override {
            return Descriptor;
        }

        auto RegisterServices(CompositionV2::FRuntimeContextV2 &context) -> void override {
            context.GetServices().Register<IFakeServiceV2>(New<FFakeServiceV2>(42));
            ++RegistrationCount;
        }

        [[nodiscard]] auto GetRegistrationCount() const -> int {
            return RegistrationCount;
        }

    private:
        CompositionV2::FModuleDescriptorV2 Descriptor;
        int RegistrationCount = 0;
    };

    class FDummyPlatformHostV2 final : public PlatformV2::IPlatformHostV2 {
    public:
        FDummyPlatformHostV2() {
            Descriptor.PlatformHostId = "dummy";
            Descriptor.DisplayName = "Dummy";
            Descriptor.bHeadless = true;
        }

        [[nodiscard]] auto GetDescriptor() const -> const PlatformV2::FPlatformHostDescriptorV2 & override {
            return Descriptor;
        }

        auto Startup() -> void override {
            bStarted = true;
        }

        auto Run() -> void override {
            bRan = true;
        }

        auto RequestStop() -> void override {
            bStopped = true;
        }

        [[nodiscard]] auto WasStarted() const -> bool {
            return bStarted;
        }

    private:
        PlatformV2::FPlatformHostDescriptorV2 Descriptor;
        bool bStarted = false;
        bool bRan = false;
        bool bStopped = false;
    };

} // namespace

TEST_CASE("Composition V2 service registry stores typed named services", "[CompositionV2]") {
    CompositionV2::FServiceRegistryV2 registry;

    registry.Register<IFakeServiceV2>(New<FFakeServiceV2>(7));
    registry.Register<IFakeServiceV2>(New<FFakeServiceV2>(11), "alternate");

    REQUIRE(registry.Count() == 2);
    REQUIRE(registry.Has<IFakeServiceV2>());
    REQUIRE(registry.Has<IFakeServiceV2>("alternate"));

    const auto defaultService = registry.Resolve<IFakeServiceV2>();
    const auto alternateService = registry.Resolve<IFakeServiceV2>("alternate");
    const auto missingService = registry.Resolve<IFakeServiceV2>("missing");

    REQUIRE(defaultService != nullptr);
    REQUIRE(alternateService != nullptr);
    REQUIRE(missingService == nullptr);
    REQUIRE(defaultService->GetValue() == 7);
    REQUIRE(alternateService->GetValue() == 11);
}

TEST_CASE("Runtime context V2 tracks platform host and installed modules", "[CompositionV2]") {
    CompositionV2::FRuntimeProfileV2 profile;
    profile.ProfileId = "lab_desktop";
    profile.DisplayName = "Lab Desktop";
    profile.BackendSelections.push_back({"plot.render", "opengl"});

    CompositionV2::FRuntimeContextV2 context(profile);
    auto platformHost = New<FDummyPlatformHostV2>();
    auto module = New<FCountingModuleV2>();

    context.SetPlatformHost(platformHost);
    context.InstallModule(module);
    context.InstallModule(module);

    REQUIRE(context.GetPlatformHost() == platformHost);
    REQUIRE(context.GetProfile().PlatformHostId == "dummy");
    REQUIRE(context.GetProfile().FindBackendId("plot.render") == "opengl");
    REQUIRE(context.GetInstalledModules().size() == 1);
    REQUIRE(context.GetProfile().ModuleIds.size() == 1);
    REQUIRE(context.GetProfile().ModuleIds.front() == "counting");

    const auto resolvedService = context.GetServices().Resolve<IFakeServiceV2>();
    REQUIRE(resolvedService != nullptr);
    REQUIRE(resolvedService->GetValue() == 42);
    REQUIRE(module->GetRegistrationCount() == 1);
}

TEST_CASE("Legacy platform host bridge exposes stable descriptors", "[CompositionV2][PlatformV2]") {
    const auto headless = PlatformV2::MakeLegacyHeadlessPlatformHostV2();
    const auto glfw = PlatformV2::MakeLegacyGLFWPlatformHostV2();
    const auto sfml = PlatformV2::MakeLegacySFMLPlatformHostV2();

    REQUIRE(headless != nullptr);
    REQUIRE(glfw != nullptr);
    REQUIRE(sfml != nullptr);

    REQUIRE(headless->GetPlatformHostId() == "headless");
    REQUIRE(headless->IsHeadless());
    REQUIRE(!headless->ProvidesGraphics());

    REQUIRE(glfw->GetLegacyBackendId() == "GLFW");
    REQUIRE(glfw->ProvidesGraphics());
    REQUIRE(glfw->ProvidesSystemWindows());
    REQUIRE(!glfw->IsHeadless());

    REQUIRE(sfml->GetLegacyBackendId() == "SFML");
    REQUIRE(sfml->ProvidesGraphics());
    REQUIRE(sfml->ProvidesSystemWindows());
}

TEST_CASE("Legacy runtime bootstrap builds explicit runtime contexts", "[CompositionV2][PlatformV2]") {
    CompositionV2::FRuntimeProfileV2 profile;
    profile.ProfileId = "studios_cli_reflect_headless";
    profile.DisplayName = "Studios CLI (Headless)";

    const auto headlessContext = CompositionV2::BuildLegacyRuntimeContextV2(
        profile,
        CompositionV2::ELegacyPlatformHostKindV2::Headless);
    const auto glfwContext = CompositionV2::BuildLegacyRuntimeContextV2(
        profile,
        CompositionV2::ELegacyPlatformHostKindV2::GLFW);

    REQUIRE(CompositionV2::GetPlatformHostIdForLegacyPlatformHostKindV2(
            CompositionV2::ELegacyPlatformHostKindV2::Headless) == "headless");
    REQUIRE(CompositionV2::GetLegacyBackendIdForPlatformHostKindV2(
            CompositionV2::ELegacyPlatformHostKindV2::GLFW) == "GLFW");

    REQUIRE(headlessContext.GetPlatformHost() != nullptr);
    REQUIRE(headlessContext.GetProfile().PlatformHostId == "headless");
    REQUIRE(headlessContext.GetPlatformHost()->IsHeadless());

    REQUIRE(glfwContext.GetPlatformHost() != nullptr);
    REQUIRE(glfwContext.GetProfile().PlatformHostId == "glfw");
    REQUIRE(glfwContext.GetPlatformHost()->ProvidesGraphics());
    REQUIRE(glfwContext.GetPlatformHost()->ProvidesSystemWindows());
}

TEST_CASE("Legacy bridge module registers reflection and task services", "[CompositionV2][ServicesV2]") {
    CompositionV2::FRuntimeProfileV2 profile;
    profile.ProfileId = "studios_cli_reflect_headless";
    profile.DisplayName = "Studios CLI (Headless)";
    auto context = CompositionV2::BuildLegacyRuntimeContextV2(
        profile,
        CompositionV2::ELegacyPlatformHostKindV2::Headless);

    context.InstallModule(CompositionV2::MakeLegacyBridgeModuleV2());

    const auto reflectionService = context.GetServices().Resolve<CompositionV2::IReflectionServiceV2>();
    const auto taskService = context.GetServices().Resolve<CompositionV2::ITaskServiceV2>();

    REQUIRE(reflectionService != nullptr);
    REQUIRE(taskService != nullptr);
    REQUIRE(reflectionService->ListSources().size() == 1);
    REQUIRE(context.GetProfile().ModuleIds.size() == 1);
    REQUIRE(context.GetProfile().ModuleIds.front() == "legacy.bridge");
}

TEST_CASE("Legacy task service is cold-safe for read-only queries", "[CompositionV2][ServicesV2]") {
    CompositionV2::FLegacyTaskServiceV2 service;

    REQUIRE_FALSE(service.HasRunningTasks());
    REQUIRE(service.GetTaskCount() == 0);
    REQUIRE(service.GetRunningTaskCount() == 0);
    REQUIRE(service.GetTaskSnapshots().empty());
    REQUIRE_FALSE(service.ClearTask(reinterpret_cast<const void *>(0x1)));
}
