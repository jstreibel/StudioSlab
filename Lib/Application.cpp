//
// Created by joao on 11/1/24.
//

#include "StudioSlab.h"

#include "Core/SlabCore.h"
#include "Core/Tools/Log.h"

#include "Application.h"

#include <utility>

namespace Slab {


    Pointer<Platform> Application::CreatePlatform() {
        return Slab::DynamicPointerCast<Graphics::GraphicBackend>(Slab::CreatePlatform("GLFW"));
    }

    Application::Application(Str name, const int argc, const char *argv[])
    : arg_count (argc)
    , arg_values( argv)
    , m_Name(std::move(name))
    {
        Startup();

        Core::Log::Info() << "Compiler: " << USED_CXX_COMPILER << Core::Log::Flush;
        // Log::Info() << "Compiler: " << COMPILER_NAME << Log::Flush;
        Core::Log::Info() << "PWD: " << Common::GetPWD() << Core::Log::Flush;
    }

    Application::~Application() {
        if (p_Platform != nullptr) p_Platform->terminate();

        Core::BackendManager::UnloadAllModules();
    }

    bool Application::Create(Resolution width, Resolution height) {
        Core::ParseCLArgs(arg_count, const_cast<const char **>(arg_values));

        p_Platform = CreatePlatform();

        OnStart();

        return true;
    }

    Int Application::Run() {
        const auto self = Dummy(*this);

        p_Platform->GetMainSystemWindow()->addEventListener(self);
        p_Platform->run();

        return 0;
    }

    void Application::SetTitle(Str title) const {
        p_Platform->GetMainSystemWindow()->setSystemWindowTitle(std::move(title));
    }

    auto Application::GetName() const -> Str {
        return m_Name;
    }

} // Slab