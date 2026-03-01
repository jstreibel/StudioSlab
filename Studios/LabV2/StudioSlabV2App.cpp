#include "StudioSlabV2App.h"

#include "LabV2WindowManager.h"

#include "Core/SlabCore.h"
#include "Graphics/Plot2D/Plot2DWindow.h"

FStudioSlabV2App::FStudioSlabV2App(const int argc, const char *argv[])
: FApplication("Studio Slab V2", argc, argv) {
}

Slab::TPointer<Slab::Graphics::FGraphicBackend> FStudioSlabV2App::CreatePlatform() {
    return Slab::DynamicPointerCast<Slab::Graphics::FGraphicBackend>(Slab::CreatePlatform("GLFW"));
}

void FStudioSlabV2App::OnStart() {
    FApplication::OnStart();

    Slab::Core::LoadModule("ModernOpenGL");

    Slab::Graphics::FPlot2DWindow::FOverlayControlsStyle plotOverlayStyle;
    plotOverlayStyle.ToolbarButtonSizeMultiplier = 3.0f;
    plotOverlayStyle.RightButtonSizeMultiplier = 3.0f;
    plotOverlayStyle.ToolbarBackgroundAlpha = 0.0f;
    plotOverlayStyle.RightBackgroundAlpha = 0.0f;
    plotOverlayStyle.EdgeMargin = 14.0f;
    plotOverlayStyle.StripPadding = 8.0f;
    plotOverlayStyle.StripSpacing = 10.0f;
    plotOverlayStyle.RightStripTopOffset = 8.0f;
    plotOverlayStyle.bTransparentBackground = true;
    plotOverlayStyle.bRightStripAlignTop = true;
    plotOverlayStyle.bRightStripAvoidDetailPanel = false;
    Slab::Graphics::FPlot2DWindow::SetGlobalOverlayControlsStyle(plotOverlayStyle);

    const auto windowManager = Slab::New<FLabV2WindowManager>();
    GetPlatform()->GetMainSystemWindow()->AddAndOwnEventListener(windowManager);
}
