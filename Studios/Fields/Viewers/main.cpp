//
// Created by joao on 7/3/24.
//

#include "Graphics/OpenGL/OpenGL.h"

#include "AppBase.h"
#include "CrashPad.h"

#include "../../../Lib/Core/Controller/Parameter/BuiltinParameters.h"

#include "Graphics/Window/WindowStyles.h"

#include "MathApp.h"

#include "Core/Tools/Log.h"

#include "Graphics/Backend/GLFW/GLFWBackend.h"
#include "Core/Backend/BackendManager.h"

#include "HistoryFileLoader.h"

#include "Graphics/DataViewers/Viewers/FourierViewer.h"
#include "Graphics/DataViewers/Viewers/HistoryViewer.h"
#include "Graphics/DataViewers/Viewers/ModesHistoryViewer.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

#include "Models/KleinGordon/RtoR/Graphics/Viewers/KGMainViewer.h"
#include "Models/KleinGordon/RtoR/Graphics/Viewers/HistoryViewer_KG.h"
#include "Models/KleinGordon/RtoR/Graphics/Viewers/HistogramsViewer_KG.h"
#include "Models/KleinGordon/RtoR/Graphics/Viewers/EnergyViewer_KG.h"
#include "Models/KleinGordon/RtoR/Graphics/Viewers/TwoPointCorrelationViewer_KG.h"
#include "Models/KleinGordon/RtoR/Graphics/Viewers/TimeFTViewer.h"

#include "Graphics/Window/SlabWindowManager.h"
#include "../../../Lib/Core/Controller/CommandLine/CommandLineArgsManager.h"
#include "Core/SlabCore.h"
#include "Graphics/DataViewers/Viewers/Viewer3D.h"

using namespace Slab;

class FApp : public Core::FAppBase {
    Core::StringParameter      filename = Core::StringParameter("", Core::FParameterDescription{'f', "filename", ".oscb file."});

public:
    FApp(int argc, const char **argv)
            : FAppBase(argc, argv, false)
    {
        Interface->AddParameters({&filename});
        Core::RegisterCLInterface(Interface);

        Core::FBackendManager::Startup("GLFW");

        Slab::Graphics::FPlotThemeManager::GetInstance();

        Core::FCLArgsManager::Parse(argc, argv);
    }

    auto run() -> int override {
        auto function = Modes::FHistoryFileLoader::Load(*filename);
        auto ddt_function = DynamicPointerCast<Slab::Math::R2toR::FNumericFunction>(function->diff(1));

        auto guiBackend = Slab::Graphics::GetGraphicsBackend();
        guiBackend->GetMainSystemWindow()->SetSystemWindowTitle(*filename);

        auto viewer = Slab::New<Slab::Models::KGRtoR::FKGMainViewer>();

        auto gui_window = viewer->getGUIWindow();
        viewer->addViewer(Slab::New<Graphics::FFourierViewer>(gui_window));
        viewer->addViewer(Slab::New<Graphics::FHistoryViewer>(gui_window));
        viewer->addKGViewer(Slab::New<Slab::Models::KGRtoR::FTimeFTViewer>(gui_window));
        viewer->addKGViewer(Slab::New<Slab::Models::KGRtoR::FHistoryViewer_KG>(gui_window));
        viewer->addKGViewer(Slab::New<Slab::Models::KGRtoR::FTwoPointCorrelationViewer_KG>(gui_window));
        viewer->addViewer(Slab::New<Graphics::FModesHistoryViewer>(gui_window));
        viewer->addKGViewer(Slab::New<Slab::Models::KGRtoR::FHistogramsViewer_KG>(gui_window));
        viewer->addKGViewer(Slab::New<Slab::Models::KGRtoR::FEnergyViewer_KG>(gui_window));
        viewer->addViewer(Slab::New<Slab::Graphics::FViewer3D>(gui_window));

        viewer->setFunction(function);
        viewer->setFunctionTimeDerivative(ddt_function);

        auto wm = Slab::New<Slab::Graphics::FSlabWindowManager>();
        wm->AddSlabWindow(viewer, false);
        guiBackend->GetMainSystemWindow()->AddEventListener(wm);

        guiBackend->Run();

        return 0;
    }
};

int run(int argc, const char** argv){
    FApp app(argc, argv);

    return app.run();
}

int main(int argc, const char* argv[]) {
    return Slab::SafetyNet::jump(
            [](int argc, const char **argv)
            {
                FApp app(argc, argv);
                return app.run();
            }, argc, argv);
}
