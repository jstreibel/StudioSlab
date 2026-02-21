//
// Created by joao on 28/09/23.
//

#ifndef STUDIOSLAB_NUKLEARSFMLMODULE_H
#define STUDIOSLAB_NUKLEARSFMLMODULE_H

#include "NuklearModule.h"

#include "Utils/Pointer.h"

#include "Graphics/Backend/SFML/SFMLListener.h"
#include "3rdParty/NuklearInclude.h"

namespace Slab::Graphics {

    class FNuklearSFMLModule : public FNuklearModule, public FSFMLListener {
        sf::RenderWindow *renderWindow = nullptr;
    public:
        FNuklearSFMLModule();
        ~FNuklearSFMLModule() override;

        void event(const sf::Event &event) override;

        void Update() override;

    };

    using NuklearSFMLModule [[deprecated("Use FNuklearSFMLModule")]] = FNuklearSFMLModule;

} // Core

#endif //STUDIOSLAB_NUKLEARSFMLMODULE_H
