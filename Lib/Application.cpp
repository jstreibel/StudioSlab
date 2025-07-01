//
// Created by joao on 11/1/24.
//

#include "StudioSlab.h"

#include "Core/SlabCore.h"
#include "Core/Tools/Log.h"

#include "Application.h"

#include <utility>

namespace Slab {


    Pointer<Platform> FApplication::CreatePlatform() {
        return Slab::DynamicPointerCast<Graphics::GraphicBackend>(Slab::CreatePlatform("GLFW"));
    }

    FApplication::FApplication(Str name, const int argc, const char *argv[])
    : ArgCount (argc)
    , ArgValues( argv)
    , m_Name(std::move(name))
    {
        Slab::Startup();

        Core::Log::Info() << "Compiler: " << USED_CXX_COMPILER << Core::Log::Flush;
        // Log::Info() << "Compiler: " << COMPILER_NAME << Log::Flush;
        Core::Log::Info() << "PWD: " << Common::GetPWD() << Core::Log::Flush;
    }

    FApplication::~FApplication() {
        if (p_Platform != nullptr) p_Platform->Terminate();

        Core::BackendManager::UnloadAllModules();
    }

    bool FApplication::Create(Resolution width, Resolution height) {
        Core::ParseCLArgs(ArgCount, const_cast<const char **>(ArgValues));

        p_Platform = CreatePlatform();

        OnStart();

        return true;
    }

    Int FApplication::Run() {
        const auto self = Dummy(*this);

        p_Platform->GetMainSystemWindow()->AddEventListener(self);
        p_Platform->Run();

        return 0;
    }

    void FApplication::SetTitle(Str title) const {
        p_Platform->GetMainSystemWindow()->SetSystemWindowTitle(std::move(title));
    }

    auto FApplication::GetName() const -> Str {
        return m_Name;
    }

} // Slab