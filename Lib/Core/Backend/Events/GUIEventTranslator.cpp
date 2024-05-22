//
// Created by joao on 27/09/23.
//

#include "GUIEventTranslator.h"

bool Core::EventTranslator::addGUIEventListener(const Core::GUIEventListener_ptr &guiEventListener) {
    if(Common::Contains(guiListeners, guiEventListener)) return false;

    guiListeners.emplace_back(guiEventListener);

    return true;
}
