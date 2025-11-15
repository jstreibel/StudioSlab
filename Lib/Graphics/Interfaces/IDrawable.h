//
// Created by joao on 11/3/25.
//

#ifndef STUDIOSLAB_IDRAWABLE_H
#define STUDIOSLAB_IDRAWABLE_H

#include "IRectProvider.h"
#include "IResolutionProvider.h"

namespace Slab::Graphics {

struct IDrawProviders {
    const IResolutionProvider& ResolutionProvider;
    const IRectRProvider& RectProvider;
};

struct IDrawable
{
    virtual ~IDrawable() = default;
    virtual void Draw(const IDrawProviders&) = 0;
};
}

#endif //STUDIOSLAB_IDRAWABLE_H