//
// Created by joao on 21/09/23.
//

#include "AppBase.h"
#include "CrashPad.h"

#include "Core/Controller/CommandLine/CommonCLParameters.h"
#include "Core/Controller/CommandLine/CLInterfaceManager.h"

#include "Graphics/Window/WindowStyles.h"

#include "Core/Tools/Log.h"

#include "Core/Backend/GLFW/GLFWBackend.h"
#include "Core/Backend/BackendManager.h"

#include "DBViewerMulti.h"
#include "DBViewerSequence.h"

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
        interface->addParameters({&snapshotDBFolders, &criticalParameter});
        Core::CLInterfaceManager::getInstance().registerInterface(interface);

        Core::BackendManager::Startup("GLFW");

        // Graphics::PlotThemeManager::G

        this->parseCLArgs();
    }

    auto run() -> int override {
        auto dbLocations = *snapshotDBFolders;

        auto &guiBackend = Core::BackendManager::GetGUIBackend();

        // auto viewer = Slab::New<Modes::DatabaseViewer::DBViewerMulti>(dbLocations, *criticalParameter);
        auto viewer = Slab::New<Modes::DatabaseViewer::DBViewerSequence>(dbLocations, *criticalParameter);

        guiBackend.addEventListener(viewer);

        guiBackend.run();

        return 0;
    }
};

int main(int argc, const char* argv[]) {
    return Slab::Core::SafetyNet::jump(
    [](int argc, const char **argv)
    {
        App app(argc, argv);
        return app.run();
    }, argc, argv);
}