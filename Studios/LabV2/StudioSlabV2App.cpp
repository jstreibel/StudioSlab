#include "StudioSlabV2App.h"

#include "LabV2WindowManager.h"

#include "Core/SlabCore.h"
#include "Core/Artifacts/V2/ArtifactStoreV2.h"
#include "Core/Composition/V2/Modules/ArtifactModuleV2.h"
#include "Graphics/Plot2D/Plot2DWindow.h"

FStudioSlabV2App::FStudioSlabV2App(const int argc, const char *argv[])
: FApplication("Studio Slab", argc, argv) {
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

    RuntimeContext = Slab::New<Slab::Core::Composition::V2::FRuntimeContextV2>();
    RuntimeContext->InstallModule(Slab::Core::Composition::V2::MakeArtifactModuleV2());
    const auto artifactStore = RuntimeContext->GetServices().Resolve<Slab::Core::Artifacts::V2::IArtifactStoreV2>();

    const auto windowManager = Slab::New<FLabV2WindowManager>(artifactStore);
    GetPlatform()->GetMainSystemWindow()->AddAndOwnEventListener(windowManager);
}
