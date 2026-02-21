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

    class FSFMLBackend : public FGraphicBackend {
        sf::Mutex off_sync;

    public:
        FSFMLBackend();

        void Run() override;

        void Terminate() override;

    protected:
        TPointer<FPlatformWindow> CreatePlatformWindow(const Str &title) override;

    public:

        static FSFMLBackend& GetInstance();
    };

    using SFMLBackend [[deprecated("Use FSFMLBackend")]] = FSFMLBackend;

}

#endif //STUDIOSLAB_SFMLBACKEND_H
