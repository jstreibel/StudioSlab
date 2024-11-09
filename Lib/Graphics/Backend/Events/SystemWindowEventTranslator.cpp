//
// Created by joao on 27/09/23.
//

#include "SystemWindowEventTranslator.h"

namespace Slab::Graphics {

    EventTranslator::EventTranslator() = default;

    bool EventTranslator::addGUIEventListener(const Volatile<SystemWindowEventListener> &guiEventListener) {
        auto in = guiEventListener.lock();

        for(IN ref : syswin_listeners) {
            auto ptr = ref.lock();
            if (ptr == in)
                return false;
        }

        syswin_listeners.emplace_back(guiEventListener);

        return true;
    }

    void EventTranslator::clear() {
        syswin_listeners.clear();
    }

}