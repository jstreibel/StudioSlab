//
// Created by joao on 29/08/2021.
//


#include "Tests/TestsApp.h"
#include <Studios/App/SafetyNet.h>


int main(int argc, const char **argv) {
    AppBase *prog = new TestsApp(argc, argv);

    return SafetyNet::jump(argc, argv, *prog);
}


