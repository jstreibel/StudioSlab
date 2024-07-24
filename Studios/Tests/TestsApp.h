//
// Created by joao on 29/08/2021.
//

#ifndef V_SHAPE_TESTSAPP_H
#define V_SHAPE_TESTSAPP_H


#include <Core/App/AppBase.h>


class TestsApp : public Slab::Core::AppBase {

public:
    TestsApp(int argc, const char**argv);

    int run() override;

};


#endif //V_SHAPE_TESTSAPP_H
