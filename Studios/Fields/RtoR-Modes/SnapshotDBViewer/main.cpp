//
// Created by joao on 21/09/23.
//

#include "Graphics/OpenGL/OpenGL.h"

#include "Core/App/AppBase.h"
#include "Core/App/CrashPad.h"

#include "Core/Controller/Interface/CommonParameters.h"
#include "Core/Controller/Interface/InterfaceManager.h"

#include "Graphics/Styles/WindowStyles.h"

#include "Core/Tools/Log.h"

#include "Core/Backend/GLFW/GLFWBackend.h"
#include "Core/Backend/Program/DummyProgram.h"
#include "Core/Backend/BackendManager.h"

#include "DBViewer.h"
#include "DatabaseParser.h"

class App : public AppBase {
        MultiStringParameter snapshotDBFolders = MultiStringParameter({"./"}, "db_folders", "the location of the snapshots "
                                                                                        "database folders");

    StringParameter criticalParameter = StringParameter("omega", "param", "the critical param of the db set; should "
                                                                          "be the only changing value both on the "
                                                                          "filenames and snapshot header");

public:
    App(int argc, const char **argv)
    : AppBase(argc, argv, false)
    {
        interface->addParameters({&snapshotDBFolders, &criticalParameter});
        InterfaceManager::getInstance().registerInterface(interface);

        Core::BackendManager::Startup(Core::GLFW);

        this->parseCLArgs();
    }

    int run() override {
        auto dbLocations = *snapshotDBFolders;

        auto &guiBackend = Core::BackendManager::GetGUIBackend();

        auto viewer = std::make_shared<Modes::DatabaseViewer::DBViewer>(dbLocations, *criticalParameter);
        guiBackend.addEventListener(viewer);

        auto program = new DummyProgram;
        guiBackend.run(program);

        delete program;

        return 0;
    }
};

int run(int argc, const char** argv){
    App app(argc, argv);

    return app.run();
}

int main(int argc, const char* argv[]) {
    return SafetyNet::jump(
    [](int argc, const char **argv)
    {
        App app(argc, argv);
        return app.run();
    }, argc, argv);
}