//
// Created by joao on 11/4/21.52c01.
//

#ifndef STUDIOSLAB_SFMLBACKEND_H
#define STUDIOSLAB_SFMLBACKEND_H

#include "Graphics/Backend/GraphicBackend.h"
#include "SFMLListener.h"
#include "SFMLEventTranslator.h"

#include <SFML/Graphics.hpp>

namespace Slab::Graphics {

    class SFMLBackend : public GraphicBackend {
        SFMLEventTranslator sfmlEventTranslator;
        sf::Mutex off_sync;

    public:
        SFMLBackend();

        void Run() override;

        void Terminate() override;

    protected:
        Pointer<SystemWindow> CreateSystemWindow(const Str &title) override;

    public:

        static SFMLBackend &GetInstance();
    };

}

#endif //STUDIOSLAB_SFMLBACKEND_H