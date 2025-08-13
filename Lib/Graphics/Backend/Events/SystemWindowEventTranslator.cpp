//
// Created by joao on 27/09/23.
//

#include "SystemWindowEventTranslator.h"

namespace Slab::Graphics {

    FEventTranslator::FEventTranslator() = default;

    bool FEventTranslator::AddGUIEventListener(const TVolatile<FPlatformWindowEventListener> &GuiEventListener) {
        const auto In = GuiEventListener.lock();

        if (ContainsReference(SysWinListeners, GuiEventListener)) return false;

        //for(IN ref : sysWin_listeners) {
        //    if (auto ptr = ref.lock(); ptr == in)
        //        return false;
        //}

        SysWinListeners.push_back(GuiEventListener);

        return true;
    }

    void FEventTranslator::clear() {
        SysWinListeners.clear();
    }

}