//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_GUIEVENTTRANSLATOR_H
#define STUDIOSLAB_GUIEVENTTRANSLATOR_H

#include <vector>
#include <utility>

#include "GUIEventListener.h"


namespace Slab::Core {

    class EventTranslator {
    protected:
        Vector<Volatile<GUIEventListener>> guiListeners;

    public:
        bool addGUIEventListener(const Volatile<GUIEventListener> &guiEventListener);

    };


}

#endif //STUDIOSLAB_GUIEVENTTRANSLATOR_H
