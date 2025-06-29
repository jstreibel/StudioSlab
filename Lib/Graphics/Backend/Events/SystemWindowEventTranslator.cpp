//
// Created by joao on 27/09/23.
//

#include "SystemWindowEventTranslator.h"

namespace Slab::Graphics {

    FEventTranslator::FEventTranslator() = default;

    bool FEventTranslator::AddGUIEventListener(const Volatile<FSystemWindowEventListener> &guiEventListener) {
        const auto in = guiEventListener.lock();

        if (ContainsReference(SysWinListeners, guiEventListener)) return false;
        //for(IN ref : sysWin_listeners) {
        //    if (auto ptr = ref.lock(); ptr == in)
        //        return false;
        //}

        SysWinListeners.push_back(guiEventListener);

        return true;
    }

    void FEventTranslator::clear() {
        SysWinListeners.clear();
    }

}