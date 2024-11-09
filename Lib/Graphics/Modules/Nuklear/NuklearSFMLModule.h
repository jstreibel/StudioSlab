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

    class NuklearSFMLModule : public NuklearModule, public SFMLListener {
        sf::RenderWindow *renderWindow = nullptr;
    public:
        NuklearSFMLModule();
        ~NuklearSFMLModule() override;

        void event(const sf::Event &event) override;

        void Update() override;

    };

} // Core

#endif //STUDIOSLAB_NUKLEARSFMLMODULE_H
