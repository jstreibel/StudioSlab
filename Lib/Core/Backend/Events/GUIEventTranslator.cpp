//
// Created by joao on 27/09/23.
//

#include "GUIEventTranslator.h"

namespace Slab::Core {

    bool EventTranslator::addGUIEventListener(const Reference<GUIEventListener> &guiEventListener) {
        auto in = guiEventListener.lock();

        for(IN ref : guiListeners) {
            auto ptr = ref.lock();
            if (ptr == in)
                return false;
        }

        guiListeners.emplace_back(guiEventListener);

        return true;
    }

}