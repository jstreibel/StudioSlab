//
// Created by joao on 29/08/2021.
//

#ifndef V_SHAPE_APPBASE_H
#define V_SHAPE_APPBASE_H

#include <Base/Controller/Interface/Interface.h>

class AppBase : protected Interface {


protected:
    AppBase(int argc, const char *argv[]);

    StrVector args;
    const int argc;
    const char **argv;

    void parseCLArgs();

public:
    virtual int run() = 0;
};


#endif //V_SHAPE_APPBASE_H
