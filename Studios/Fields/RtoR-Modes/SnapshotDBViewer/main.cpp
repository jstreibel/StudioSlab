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
    StringParameter snapshotDBFolder = StringParameter("./db", "db_folder", "the location of the snapshots "
                                                                            "database folder");

    StringParameter criticalParameter = StringParameter("omega", "param", "the critical param of the db set; should "
                                                                          "be the only changing value both on the "
                                                                          "filenames and snapshot header");

public:
    App(int argc, const char **argv)
    : AppBase(argc, argv, false)
    {
        interface->addParameters({&snapshotDBFolder, &criticalParameter});
        InterfaceManager::getInstance().registerInterface(interface);

        Core::BackendManager::Startup(Core::GLFW);

        this->parseCLArgs();
    }

    int run() override {
        Str dbLocation = Common::GetPWD() + "/" + *snapshotDBFolder;

        auto &guiBackend = Core::BackendManager::GetGUIBackend();

        auto parser = std::make_shared<Modes::DatabaseViewer::DBParser>(dbLocation, *criticalParameter);
        auto viewer = std::make_shared<Modes::DatabaseViewer::DBViewer>(parser);
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