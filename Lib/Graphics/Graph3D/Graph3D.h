//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_GRAPH3D_H
#define STUDIOSLAB_GRAPH3D_H

#include "Graphics/Window/Window.h"
#include "Actor.h"

#include <glm/detail/type_mat4x4.hpp>

#include <list>

namespace Graphics {

    class Graph3D : public Window {
        glm::mat4 projection{};
        glm::mat4 modelview{};

        std::list<Actor::Ptr> actors;

    public:
        Graph3D();
        void draw() override;
    };

} // Graphics

#endif //STUDIOSLAB_GRAPH3D_H
