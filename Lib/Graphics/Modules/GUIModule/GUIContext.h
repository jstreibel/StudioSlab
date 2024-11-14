//
// Created by joao on 11/3/24.
//

#ifndef STUDIOSLAB_GUICONTEXT_H
#define STUDIOSLAB_GUICONTEXT_H

#include "Graphics/Backend/Events/SystemWindowEventListener.h"

namespace Slab::Graphics {

    using DrawCall = std::function<void(void)>;

    class GUIContext : public SystemWindowEventListener {
    protected:
        Vector<DrawCall> draw_calls;

        void FlushDrawCalls();
    public:
        virtual void NewFrame() = 0;

        void AddDrawCall(const DrawCall&);
    };

} // Slab::Graphics

#endif //STUDIOSLAB_GUICONTEXT_H
