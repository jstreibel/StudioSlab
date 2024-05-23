//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_ACTOR_H
#define STUDIOSLAB_ACTOR_H

#include <memory>

namespace Slab::Graphics {

    class Graph3D;

    class Actor {
    public:
        typedef std::shared_ptr<Actor> Ptr;

        virtual void draw(const Graph3D &graph3D) = 0;
    };

} // Graphics

#endif //STUDIOSLAB_ACTOR_H
