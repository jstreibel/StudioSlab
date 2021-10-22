//
// Created by joao on 29/08/2021.
//

#include <Apps/Backend/GLUTBackend.h>
#include "TestsApp.h"
#include "OpenGLView.h"

TestsApp:: TestsApp(int argc, const char**argv) : AppBase(argc, argv) {}


int TestsApp::run() {
    CLOptionsDescription options = buildFullCommandLineOptions();

    CLVariablesMap vm; namespace po = boost::program_options;
    po::store(po::parse_command_line(argc, argv, options), vm);

    auto view = OpenGLView();
    auto pDummyProgram = new DummyProgram;

    auto backend = GLUTBackend::getSingleton();

    backend->setOpenGLOutput(&view);

    backend->run(pDummyProgram);


    return 0;
}

CLOptionsDescription TestsApp::buildFullCommandLineOptions() {
    CLOptionsDescription general("Test options.");
    general.add_options()("help", "Help.");

    return CLOptionsDescription();
}
