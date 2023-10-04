//
// Created by joao on 8/6/23.
//

#ifndef STUDIOSLAB_APP_H
#define STUDIOSLAB_APP_H


#include "Core/App/AppBase.h"
#include "Builder.h"
#include "Core/Backend/SFML/SFMLBackend.h"

namespace MolecularDynamics {
    class App : public AppBase {
        MolecularDynamics::Builder builder;
        SFMLBackend &backend;
    public:
        App(int argc, const char *argv[]);

        int run() override;
    };
}


#endif //STUDIOSLAB_APP_H
