//
// Created by joao on 7/3/24.
//

#include "Graphics/OpenGL/OpenGL.h"

#include "Core/App/AppBase.h"
#include "Core/App/CrashPad.h"

#include "Core/Controller/Parameter/CommonParameters.h"
#include "Core/Controller/Interface/InterfaceManager.h"

#include "Graphics/Window/WindowStyles.h"

#include "Math/App.h"

#include "Core/Tools/Log.h"

#include "Core/Backend/GLFW/GLFWBackend.h"
#include "Core/Backend/BackendManager.h"

#include "HistoryFileLoader.h"

#include "Graphics/DataViewers/Viewers/FourierViewer.h"
#include "Graphics/DataViewers/Viewers/HistoryViewer.h"
#include "Graphics/DataViewers/Viewers/ModesHistoryViewer.h"
#include "Graphics/Graph/PlotThemeManager.h"

#include "Models/KleinGordon/RtoR/Graphics/Viewers/KGMainViewer.h"
#include "Models/KleinGordon/RtoR/Graphics/Viewers/HistoryViewer_KG.h"
#include "Models/KleinGordon/RtoR/Graphics/Viewers/HistogramsViewer_KG.h"
#include "Models/KleinGordon/RtoR/Graphics/Viewers/EnergyViewer_KG.h"
#include "Models/KleinGordon/RtoR/Graphics/Viewers/TwoPointCorrelationViewer_KG.h"

using namespace Slab;

class App : public Math::AppBase {
    Core::StringParameter      filename = Core::StringParameter("", "filename,f", ".oscb file.");

public:
    App(int argc, const char **argv)
            : AppBase(argc, argv, false)
    {
        interface->addParameters({&filename});
        Core::InterfaceManager::getInstance().registerInterface(interface);

        Core::BackendManager::Startup(Core::GLFW);

        Slab::Graphics::PlotThemeManager::GetInstance();

        this->parseCLArgs();
    }

    auto run() -> int override {
        auto function = Modes::HistoryFileLoader::Load(*filename);
        auto ddt_function = DynamicPointerCast<Slab::Math::R2toR::NumericFunction>(function->diff(1));

        auto &guiBackend = Core::BackendManager::GetGUIBackend();
        guiBackend.setSystemWindowTitle(*filename);

        auto viewer = Slab::New<Slab::Models::KGRtoR::KGMainViewer>();

        auto gui_window = viewer->getGUIWindow();
        viewer->addViewer(Slab::New<Graphics::FourierViewer>(gui_window));
        viewer->addViewer(Slab::New<Graphics::HistoryViewer>(gui_window));
        viewer->addKGViewer(Slab::New<Slab::Models::KGRtoR::HistoryViewer>(gui_window));
        viewer->addKGViewer(Slab::New<Slab::Models::KGRtoR::TwoPointCorrelationViewer_KG>(gui_window));
        viewer->addViewer(Slab::New<Graphics::ModesHistoryViewer>(gui_window));
        viewer->addKGViewer(Slab::New<Slab::Models::KGRtoR::HistogramsViewer_KG>(gui_window));
        viewer->addKGViewer(Slab::New<Slab::Models::KGRtoR::EnergyViewer_KG>(gui_window));

        viewer->setFunction(function);
        viewer->setFunctionTimeDerivative(ddt_function);

        guiBackend.addEventListener(viewer);

        guiBackend.run();

        return 0;
    }
};

int run(int argc, const char** argv){
    App app(argc, argv);

    return app.run();
}

int main(int argc, const char* argv[]) {
    return Slab::Core::SafetyNet::jump(
            [](int argc, const char **argv)
            {
                App app(argc, argv);
                return app.run();
            }, argc, argv);
}