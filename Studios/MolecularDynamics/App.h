//
// Created by joao on 8/6/23.
//

#ifndef STUDIOSLAB_MOLDYN_APP_H
#define STUDIOSLAB_MOLDYN_APP_H


#include "AppBase.h"
#include "Models/MolecularDynamics/MolecularDynamicsRecipe.h"
#include "Graphics/Backend/SFML/SFMLBackend.h"
#include "Math/Numerics/NumericTask.h"

namespace Studios::MolecularDynamics {

    class FApp : public Slab::Core::FAppBase {
        Slab::TPointer<Slab::Math::FNumericTask> NumericTask;
    public:
        FApp(int argc, const char *argv[]);

        int run() override;
    };

    using App [[deprecated("Use FApp")]] = FApp;
}


#endif //STUDIOSLAB_MOLDYN_APP_H
