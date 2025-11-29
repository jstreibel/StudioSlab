//
// Created by joao on 11/20/25.
//

#ifndef STUDIOSLAB_FSCENE_H
#define STUDIOSLAB_FSCENE_H
#include "../Interfaces.h"
#include "Graphics/Interfaces/IDrawable.h"

using namespace Slab;

struct IScene : Graphics::IDrawable, IUpdateable {
    virtual void Startup(const Graphics::FPlatformWindow&) = 0;
    virtual Graphics::RectR GetCurrentView() const = 0;
};

#endif //STUDIOSLAB_FSCENE_H