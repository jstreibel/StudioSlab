//
// Created by joao on 29/09/23.
//

#ifndef STUDIOSLAB_NUKLEARTESTS_H
#define STUDIOSLAB_NUKLEARTESTS_H

#include "Graphics/Backend/SFML/SFMLListener.h"
#include "Graphics/Backend/Events/SystemWindowEventListener.h"

namespace Tests {

    class NuklearTests : public Slab::Graphics::FPlatformWindowEventListener{
    public:
        NuklearTests();

        bool NotifyRender(const Slab::Graphics::FPlatformWindow&) override;
    };

} // Tests

#endif //STUDIOSLAB_NUKLEARTESTS_H
