//
// Created by joao on 11/15/25.
//

#ifndef STUDIOSLAB_IRECTPROVIDER_H
#define STUDIOSLAB_IRECTPROVIDER_H

#include "Graphics/Types2D.h"

namespace Slab::Graphics {
struct IRectRProvider {
    virtual ~IRectRProvider() = default;
    virtual RectR GetRect() const = 0;
};
}


#endif //STUDIOSLAB_IRECTPROVIDER_H