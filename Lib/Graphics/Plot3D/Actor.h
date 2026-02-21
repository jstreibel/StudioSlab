//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_ACTOR_H
#define STUDIOSLAB_ACTOR_H

#include <memory>
#include "Utils/Pointer.h"
#include "Utils/String.h"

namespace Slab::Graphics {

    class FScene3DWindow;

    class FActor {
        Str label = "<unnamed actor>";
        bool visible = true;

    public:
        virtual void draw(const FScene3DWindow &graph3D) = 0;
        virtual bool hasGUI();
        virtual void drawGUI();

        void setLabel(Str);
        auto getLabel() const -> Str;

        void setVisibility(bool);
        bool isVisible() const;
    };

    DefinePointers(FActor)

    using Actor [[deprecated("Use FActor")]] = FActor;
    using Actor_ptr [[deprecated("Use FActor_ptr")]] = FActor_ptr;
    using Actor_constptr [[deprecated("Use FActor_constptr")]] = FActor_constptr;
    using Actor_ref [[deprecated("Use FActor_ref")]] = FActor_ref;
    using Actor_constref [[deprecated("Use FActor_constref")]] = FActor_constref;

} // Graphics

#endif //STUDIOSLAB_ACTOR_H
