//
// Created by joao on 29/09/23.
//

#ifndef STUDIOSLAB_NUKLEARTESTS_H
#define STUDIOSLAB_NUKLEARTESTS_H

#include "Core/Backend/SFML/SFMLListener.h"
#include "Core/Backend/Events/GUIEventListener.h"

namespace Tests {

    class NuklearTests : public Slab::Core::GUIEventListener{
    public:
        NuklearTests();

        bool notifyRender() override;
    };

} // Tests

#endif //STUDIOSLAB_NUKLEARTESTS_H
