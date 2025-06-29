//
// Created by joao on 27/09/23.
//

#include "SystemWindowEventTranslator.h"

namespace Slab::Graphics {

    FEventTranslator::FEventTranslator() = default;

    bool FEventTranslator::AddGUIEventListener(const Volatile<FSystemWindowEventListener> &guiEventListener) {
        const auto in = guiEventListener.lock();

        if (ContainsReference(sysWin_listeners, guiEventListener)) return false;
        //for(IN ref : sysWin_listeners) {
        //    if (auto ptr = ref.lock(); ptr == in)
        //        return false;
        //}

        sysWin_listeners.push_back(guiEventListener);

        return true;
    }

    void FEventTranslator::clear() {
        sysWin_listeners.clear();
    }

}