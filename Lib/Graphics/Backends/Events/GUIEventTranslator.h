//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_GUIEVENTTRANSLATOR_H
#define STUDIOSLAB_GUIEVENTTRANSLATOR_H

#include <vector>
#include <utility>

#include "GUIEventListener.h"


namespace Core {
    class EventTranslator {
    protected:
        std::vector<Core::GUIEventListener::Ptr> guiListeners;

    public:
        bool addGUIEventListener(const Core::GUIEventListener::Ptr& guiEventListener);

    };


}

#endif //STUDIOSLAB_GUIEVENTTRANSLATOR_H
