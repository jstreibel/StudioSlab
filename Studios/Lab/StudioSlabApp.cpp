//
// Created by joao on 8/14/25.
//

#include "StudioSlabApp.h"

#include "StudioWindowManager.h"
#include "Graphics/Plot2D/Plot2DWindow.h"

StudioSlabApp::StudioSlabApp(int argc, const char* argv[]): FApplication("Studio Slab", argc, argv)
{

}

void StudioSlabApp::OnStart()
{
    FApplication::OnStart();

    const auto WindowManager = Slab::New<StudioWindowManager>();
    this->GetPlatform()->GetMainSystemWindow()->AddAndOwnEventListener(WindowManager);

    auto Window = Slab::New<Slab::Graphics::FPlot2DWindow>("Studio Slab");

    WindowManager->AddSlabWindow(Window, false);
}


