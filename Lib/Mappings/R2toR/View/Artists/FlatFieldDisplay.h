//
// Created by joao on 4/8/23.
//

#ifndef STUDIOSLAB_FLATFIELDDISPLAY_H
#define STUDIOSLAB_FLATFIELDDISPLAY_H

#include "Base/Graphics/Window/Window.h"
#include "Base/Graphics/OpenGL/Texture.h"
#include "Mappings/R2toR/Model/R2toRFunction.h"

namespace R2toR::Graphics {
    class FlatFieldDisplay : public Window {
        R2toR::Function::ConstPtr func = nullptr;

        OpenGL::Texture texture;

    public:
        explicit FlatFieldDisplay(R2toR::Function::ConstPtr function, Resolution w, Resolution h);

        void draw() override;
    };
}



#endif //STUDIOSLAB_FLATFIELDDISPLAY_H
