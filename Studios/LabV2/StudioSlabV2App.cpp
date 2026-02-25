#include "StudioSlabV2App.h"

#include "LabV2WindowManager.h"

#include "Core/SlabCore.h"

FStudioSlabV2App::FStudioSlabV2App(const int argc, const char *argv[])
: FApplication("Studio Slab V2", argc, argv) {
}

Slab::TPointer<Slab::Graphics::FGraphicBackend> FStudioSlabV2App::CreatePlatform() {
    return Slab::DynamicPointerCast<Slab::Graphics::FGraphicBackend>(Slab::CreatePlatform("GLFW"));
}

void FStudioSlabV2App::OnStart() {
    FApplication::OnStart();

    Slab::Core::LoadModule("ModernOpenGL");

    const auto windowManager = Slab::New<FLabV2WindowManager>();
    GetPlatform()->GetMainSystemWindow()->AddAndOwnEventListener(windowManager);
}
