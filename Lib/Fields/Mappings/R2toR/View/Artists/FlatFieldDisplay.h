//
// Created by joao on 4/8/23.
//

#ifndef STUDIOSLAB_FLATFIELDDISPLAY_H
#define STUDIOSLAB_FLATFIELDDISPLAY_H

#include "Base/Graphics/WindowManagement/Window.h"

namespace R2toR {
    namespace Graphics {
        class FlatFieldDisplay : public Window {
        public:
            void draw() override;
        };
    }
}


#endif //STUDIOSLAB_FLATFIELDDISPLAY_H
