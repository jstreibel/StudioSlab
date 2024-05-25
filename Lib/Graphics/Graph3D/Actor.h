//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_ACTOR_H
#define STUDIOSLAB_ACTOR_H

#include <memory>
#include "Utils/Pointer.h"
#include "Utils/String.h"

namespace Slab::Graphics {

    class Scene3DWindow;

    class Actor {
        Str label = "<unnamed actor>";
        bool visible = true;

    public:
        virtual void draw(const Scene3DWindow &graph3D) = 0;
        virtual bool hasGUI();
        virtual void drawGUI();

        void setLabel(Str);
        auto getLabel() const -> Str;

        void setVisibility(bool);
        bool isVisible() const;
    };

    DefinePointer(Actor)

} // Graphics

#endif //STUDIOSLAB_ACTOR_H
