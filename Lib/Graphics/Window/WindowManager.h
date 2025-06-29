//
// Created by joao on 6/28/25.
//

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include "SlabWindow.h"
#include "Graphics/Backend/Events/SystemWindowEventListener.h"

namespace Slab::Graphics {

    class FWindowManager : public FSystemWindowEventListener {
        public:
        FWindowManager() = delete;
        explicit FWindowManager(SystemWindow *Parent);

        void AddSlabWindow(const Pointer<FSlabWindow>&);
        virtual void AddSlabWindow(const Pointer<FSlabWindow>&, bool hidden) = 0;
    };

} // Slab::Graphics

#endif //WINDOWMANAGER_H
