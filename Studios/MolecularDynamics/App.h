//
// Created by joao on 8/6/23.
//

#ifndef STUDIOSLAB_APP_H
#define STUDIOSLAB_APP_H


#include "AppBase.h"
#include "Recipe.h"
#include "Core/Backend/SFML/SFMLBackend.h"
#include "Math/Numerics/NumericTask.h"

namespace MolecularDynamics {

    using namespace Slab::Core;

    class App : public AppBase {
        Pointer<Slab::Math::NumericTask> numericTask;
    public:
        App(int argc, const char *argv[]);

        int run() override;
    };
}


#endif //STUDIOSLAB_APP_H
