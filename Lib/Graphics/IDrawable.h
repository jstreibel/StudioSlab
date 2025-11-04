//
// Created by joao on 11/3/25.
//

#ifndef STUDIOSLAB_IDRAWABLE_H
#define STUDIOSLAB_IDRAWABLE_H

namespace Slab::Graphics {
class IDrawable
{
public:
    virtual ~IDrawable() = default;
    virtual void Draw() = 0;
};
}

#endif //STUDIOSLAB_IDRAWABLE_H