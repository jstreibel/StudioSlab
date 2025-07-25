//
// Created by joao on 8/6/23.
//

#ifndef STUDIOSLAB_MOLDYN_APP_H
#define STUDIOSLAB_MOLDYN_APP_H


#include "AppBase.h"
#include "Models/MolecularDynamics/Recipe.h"
#include "Graphics/Backend/SFML/SFMLBackend.h"
#include "Math/Numerics/NumericTask.h"

namespace Studios::MolecularDynamics {

    class App : public Slab::Core::AppBase {
        Slab::TPointer<Slab::Math::NumericTask> numericTask;
    public:
        App(int argc, const char *argv[]);

        int run() override;
    };
}


#endif //STUDIOSLAB_MOLDYN_APP_H
