//
// Created by joao on 29/08/2021.
//

#ifndef V_SHAPE_APPBASE_H
#define V_SHAPE_APPBASE_H

#include <Base/Controller/Interface/InterfaceOwner.h>

class AppBase : public InterfaceOwner {

protected:
    AppBase(int argc, const char *argv[], bool doRegister=true);
    virtual ~AppBase();

    void parseCLArgs();

public:
    virtual int run() = 0;
};


#endif //V_SHAPE_APPBASE_H
