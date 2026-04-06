//
// Created by joao on 29/08/2021.
//

#ifndef V_SHAPE_APPBASE_H
#define V_SHAPE_APPBASE_H

#include "Core/Controller/InterfaceOwner.h"


namespace Slab::Core {

    class FAppBase : public FInterfaceOwner {

    protected:
        FAppBase(int argc, const char *argv[], bool doRegister = true);

        virtual ~FAppBase();

    public:
        virtual int run() = 0;
    };

    using AppBase [[deprecated("Use FAppBase")]] = FAppBase;


}

#endif //V_SHAPE_APPBASE_H
