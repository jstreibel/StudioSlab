//
// Created by joao on 21/09/23.
//

#include "AppBase.h"
#include "CrashPad.h"

#include "../../../../Lib/Core/Controller/Parameter/BuiltinParameters.h"
#include "../../../../Lib/Core/Controller/InterfaceManager.h"

#include "Graphics/Window/WindowStyles.h"

#include "Core/Tools/Log.h"

#include "Graphics/Backend/GLFW/GLFWBackend.h"
#include "Core/Backend/BackendManager.h"

#include "DBViewerMulti.h"
#include "DBViewerSequence.h"
#include "Graphics/Window/SlabWindowManager.h"
#include "../../../../Lib/Core/Controller/CommandLine/CommandLineArgsManager.h"
#include "Core/SlabCore.h"
#include "Graphics/SlabGraphics.h"

using namespace Slab;

class FApp : public Core::FAppBase {
    Core::MultiStringParameter snapshotDBFolders = Core::MultiStringParameter({"./"}, Core::FParameterDescription{"db_folders", "the location of the snapshots database folders"});
    Core::StringParameter      criticalParameter = Core::StringParameter("harmonic",  Core::FParameterDescription{"param", "the critical param of the db set; should be the only changing value both on the filenames and snapshot header"});

public:
    FApp(int argc, const char **argv)
    : FAppBase(argc, argv, false)
    {
        Interface->AddParameters({&snapshotDBFolders, &criticalParameter});
        Core::RegisterCLInterface(Interface);

        Core::FBackendManager::Startup("GLFW");

        // Graphics::FPlotThemeManager::G

        Core::ParseCLArgs(argc, argv);
    }

    auto run() -> int override {
        auto dbLocations = *snapshotDBFolders;

        const auto GuiBackend = Slab::Graphics::GetGraphicsBackend();

        // auto viewer = Slab::New<Modes::DatabaseViewer::FDBViewerMulti>(dbLocations, *criticalParameter);
        const auto Viewer = Slab::New<Modes::DatabaseViewer::FDBViewerSequence>(dbLocations, *criticalParameter);


        const auto WindowManager = Slab::New<Graphics::FSlabWindowManager>();
        GuiBackend->GetMainSystemWindow()->AddEventListener(WindowManager);

        WindowManager->AddSlabWindow(Viewer, false);
        GuiBackend->Run();

        return 0;
    }
};

int main(int argc, const char* argv[]) {
    return Slab::SafetyNet::jump(
    [](int argc, const char **argv)
    {
        FApp app(argc, argv);
        return app.run();
    }, argc, argv);
}
