//
// Created by joao on 29/08/2021.
//

#ifndef V_SHAPE_TESTSAPP_H
#define V_SHAPE_TESTSAPP_H


#include "Lib/App/AppBase.h"


class TestsApp : public AppBase {

public:
    TestsApp(int argc, const char**argv);

    int run() override;

    CLOptionsDescription buildFullCommandLineOptions() override;

};


#endif //V_SHAPE_TESTSAPP_H