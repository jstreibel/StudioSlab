//
// Created by joao on 12/7/25.
//

#ifndef STUDIOSLAB_IDRAWBACKEND2DPROVIDER_H
#define STUDIOSLAB_IDRAWBACKEND2DPROVIDER_H
#include "IDrawBackend2D.h"
#include "Utils/Pointer.h"

namespace Slab::Graphics {
struct IDrawBackend2DProvider {
    virtual ~IDrawBackend2DProvider() = default;
    virtual TPointer<IDrawBackend2D> GetRenderer() const = 0;
};
}

#endif //STUDIOSLAB_IDRAWBACKEND2DPROVIDER_H