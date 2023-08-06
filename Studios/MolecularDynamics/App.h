//
// Created by joao on 8/6/23.
//

#ifndef STUDIOSLAB_APP_H
#define STUDIOSLAB_APP_H


#include "Base/App/AppBase.h"

namespace MolecularDynamics {
    class App : public AppBase {
    public:
        App(int argc, const char *argv[]);

        int run() override;
    };
}


#endif //STUDIOSLAB_APP_H
