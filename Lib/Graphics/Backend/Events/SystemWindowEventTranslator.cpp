//
// Created by joao on 27/09/23.
//

#include "SystemWindowEventTranslator.h"

namespace Slab::Graphics {

    EventTranslator::EventTranslator() = default;

    bool EventTranslator::addGUIEventListener(const Volatile<SystemWindowEventListener> &guiEventListener) {
        const auto in = guiEventListener.lock();

        for(IN ref : sysWin_listeners) {
            if (auto ptr = ref.lock(); ptr == in)
                return false;
        }

        sysWin_listeners.push_back(guiEventListener);

        return true;
    }

    void EventTranslator::clear() {
        sysWin_listeners.clear();
    }

}