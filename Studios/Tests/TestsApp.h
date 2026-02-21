//
// Created by joao on 29/08/2021.
//

#ifndef V_SHAPE_TESTSAPP_H
#define V_SHAPE_TESTSAPP_H


#include "AppBase.h"


class FTestsApp : public Slab::Core::FAppBase {

public:
    FTestsApp(int argc, const char**argv);

    int run() override;

};

using TestsApp [[deprecated("Use FTestsApp")]] = FTestsApp;


#endif //V_SHAPE_TESTSAPP_H
