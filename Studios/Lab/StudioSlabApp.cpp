//
// Created by joao on 8/14/25.
//

#include "StudioSlabApp.h"

#include "LabWindowManager.h"
#include "Core/SlabCore.h"
#include "Graphics/Plot2D/Plot2DWindow.h"

FLittlePlaneDesignerApp::FLittlePlaneDesignerApp(int argc, const char* argv[]) : FApplication("Studio Slab", argc, argv)
{

}

Slab::TPointer<Slab::Graphics::GraphicBackend> FLittlePlaneDesignerApp::CreatePlatform()
{
    // return Slab::DynamicPointerCast<Slab::Graphics::GraphicBackend>(Slab::CreatePlatform("SFML"));
    return Slab::DynamicPointerCast<Slab::Graphics::GraphicBackend>(Slab::CreatePlatform("GLFW"));
}

void FLittlePlaneDesignerApp::OnStart()
{
    FApplication::OnStart();

    Slab::Core::LoadModule("ModernOpenGL");

    const auto WindowManager = Slab::New<StudioWindowManager>();
    this->GetPlatform()->GetMainSystemWindow()->AddAndOwnEventListener(WindowManager);
}


