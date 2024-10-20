//
// Created by joao on 27/09/23.
//

#include "SystemWindowEventTranslator.h"

namespace Slab::Graphics {

    bool EventTranslator::addGUIEventListener(const Volatile<SystemWindowEventListener> &guiEventListener) {
        auto in = guiEventListener.lock();

        for(IN ref : guiListeners) {
            auto ptr = ref.lock();
            if (ptr == in)
                return false;
        }

        guiListeners.emplace_back(guiEventListener);

        return true;
    }

    void EventTranslator::clear() {
        guiListeners.clear();
    }

}