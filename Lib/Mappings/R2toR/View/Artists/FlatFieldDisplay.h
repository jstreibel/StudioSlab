//
// Created by joao on 4/8/23.
//

#ifndef STUDIOSLAB_FLATFIELDDISPLAY_H
#define STUDIOSLAB_FLATFIELDDISPLAY_H

#include "Base/Graphics/Window/Window.h"
#include "Base/Graphics/OpenGL/Texture.h"

#include "Phys/Graph/Graph.h"
#include "Mappings/R2toR/Model/R2toRFunction.h"

namespace R2toR::Graphics {
class FlatFieldDisplay : public Base::Graphics::Graph2D {
        bool validBuffer = false;

    protected:
        R2toR::Function::ConstPtr func    = nullptr;
        OpenGL::Texture*          texture = nullptr;

        Styles::Color computeColor(Real val) const;
    public:
        FlatFieldDisplay(Str title="Full 2D") : Base::Graphics::Graph2D(-1,1,-1,1,title) {};
        explicit FlatFieldDisplay(R2toR::Function::ConstPtr function);

        void setup(R2toR::Function::ConstPtr function);

        virtual void draw() override;

        void invalidateBuffer();

        void repopulateBuffer();
    };
}



#endif //STUDIOSLAB_FLATFIELDDISPLAY_H
