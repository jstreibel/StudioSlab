//
// Created by joao on 21/09/23.
//

#include "AppBase.h"
#include "CrashPad.h"

#include "Core/Controller/CommandLine/CommonCLParameters.h"
#include "Core/Controller/CommandLine/CLInterfaceManager.h"

#include "Graphics/Window/WindowStyles.h"

#include "Core/Tools/Log.h"

#include "Graphics/Backend/GLFW/GLFWBackend.h"
#include "Core/Backend/BackendManager.h"

#include "DBViewerMulti.h"
#include "DBViewerSequence.h"
#include "Graphics/Window/SlabWindowManager.h"
#include "Core/Controller/CommandLine/CLArgsManager.h"
#include "Core/SlabCore.h"

using namespace Slab;

class App : public Core::AppBase {
    Core::MultiStringParameter snapshotDBFolders = Core::MultiStringParameter({"./"}, "db_folders", "the location of the snapshots "
                                                                                        "database folders");

    Core::StringParameter      criticalParameter = Core::StringParameter("harmonic", "param",
                                                                         "the critical param of the db set; should "
                                                                          "be the only changing value both on the "
                                                                          "filenames and snapshot header");

public:
    App(int argc, const char **argv)
    : AppBase(argc, argv, false)
    {
        Interface->addParameters({&snapshotDBFolders, &criticalParameter});
        Core::RegisterCLInterface(Interface);

        Core::BackendManager::Startup("GLFW");

        // Graphics::PlotThemeManager::G

        Core::ParseCLArgs(argc, argv);
    }

    auto run() -> int override {
        auto dbLocations = *snapshotDBFolders;

        const auto guiBackend = Slab::Graphics::GetGraphicsBackend();

        // auto viewer = Slab::New<Modes::DatabaseViewer::DBViewerMulti>(dbLocations, *criticalParameter);
        const auto viewer = Slab::New<Modes::DatabaseViewer::DBViewerSequence>(dbLocations, *criticalParameter);

        const auto wm = Slab::New<Slab::Graphics::SlabWindowManager>(guiBackend->GetMainSystemWindow().get());
        wm->addSlabWindow(viewer);
        guiBackend->GetMainSystemWindow()->addEventListener(wm);

        guiBackend->run();

        return 0;
    }
};

int main(int argc, const char* argv[]) {
    return Slab::SafetyNet::jump(
    [](int argc, const char **argv)
    {
        App app(argc, argv);
        return app.run();
    }, argc, argv);
}