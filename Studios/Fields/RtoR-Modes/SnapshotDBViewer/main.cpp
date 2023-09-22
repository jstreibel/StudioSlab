//
// Created by joao on 21/09/23.
//

#include "Core/App/AppBase.h"
#include "Core/App/CrashPad.h"
#include "Core/Controller/Interface/CommonParameters.h"
#include "Core/Controller/Interface/InterfaceManager.h"
#include "Core/Tools/Log.h"

#include "DBViewer.h"
#include "Core/Backend/GLFW/GLFWBackend.h"
#include "Core/Backend/DummyProgram.h"

class App : public AppBase {
    StringParameter snapshotDBFolder = StringParameter("./snapshots", "db_folder", "The location of the snapshots "
                                                                            "database folder");
public:
    App(int argc, const char **argv)
    : AppBase(argc, argv, false)
    {
        interface->addParameters({&snapshotDBFolder});
        InterfaceManager::getInstance().registerInterface(interface);

        Backend::Initialize<GLFWBackend>();

        this->parseCLArgs();
    }

    int run() override {
        Str dbLocation = Common::GetPWD() + "/" + *snapshotDBFolder;

        Log::Info() << "Snapshots database location: " << dbLocation << Log::Flush;

        auto program = new DummyProgram;
        GUIBackend::GetInstance().run(program);

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