//
// Created by joao on 11/3/24.
//

#ifndef STUDIOSLAB_GUICONTEXT_H
#define STUDIOSLAB_GUICONTEXT_H

namespace Slab::Graphics {

    class GUIContext {
    public:
        virtual void NewFrame() = 0;
        virtual void EndFrame(bool render=true) = 0;

    };

} // Slab::Graphics

#endif //STUDIOSLAB_GUICONTEXT_H
