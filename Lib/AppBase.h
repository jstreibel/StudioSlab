//
// Created by joao on 29/08/2021.
//

#ifndef V_SHAPE_APPBASE_H
#define V_SHAPE_APPBASE_H

#include "Core/Controller/CommandLine/CLInterfaceOwner.h"


namespace Slab::Core {

    class AppBase : public CLInterfaceOwner {

    protected:
        AppBase(int argc, const char *argv[], bool doRegister = true);

        virtual ~AppBase();

    public:
        virtual int run() = 0;
    };


}

#endif //V_SHAPE_APPBASE_H
