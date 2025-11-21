//
// Created by joao on 11/15/25.
//

#ifndef STUDIOSLAB_IRESOLUTIONPROVIDER_H
#define STUDIOSLAB_IRESOLUTIONPROVIDER_H
#include "Utils/Numbers.h"

namespace Slab::Graphics {
struct IResolutionProvider {
    virtual ~IResolutionProvider() = default;

    [[nodiscard]] virtual Int GetHeight() const = 0;
    [[nodiscard]] virtual Int GetWidth() const = 0;
};
}

#endif //STUDIOSLAB_IRESOLUTIONPROVIDER_H