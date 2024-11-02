//
// Created by joao on 11/1/24.
//

#include "StudioSlab.h"

#include "Core/SlabCore.h"
#include "Core/Tools/Log.h"

#include "Application.h"

#include <utility>

namespace Slab {
    int arg_count;
    char **arg_values;

    Application::Application(Str name, int argc, char *argv[])
    : m_Name(std::move(name))
    {
        Slab::Startup();

        arg_count = argc;
        arg_values = argv;

        Core::Log::Info() << "Compiler: " << USED_CXX_COMPILER << Core::Log::Flush;
        // Log::Info() << "Compiler: " << COMPILER_NAME << Log::Flush;
        Core::Log::Info() << "PWD: " << Common::GetPWD() << Core::Log::Flush;
    }

    Application::~Application() {
        m_Platform->terminate();
        Core::BackendManager::UnloadAllModules();
    }

    bool Application::Create(Resolution width, Resolution height) {
        Slab::Core::ParseCLArgs(arg_count, const_cast<const char **>(arg_values));

        m_Platform = CreatePlatform();

        OnStart();

        return true;
    }

    Int Application::Run() {
        auto self = Dummy(*this);

        m_Platform->addEventListener(self);
        m_Platform->run();

        return 0;
    }

    void Application::SetTitle(Str title) {
        m_Platform->setSystemWindowTitle(std::move(title));
    }

    auto Application::GetName() const -> Str {
        return m_Name;
    }

} // Slab