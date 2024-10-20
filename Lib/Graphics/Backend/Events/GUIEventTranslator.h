//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_GUIEVENTTRANSLATOR_H
#define STUDIOSLAB_GUIEVENTTRANSLATOR_H

#include <vector>
#include <utility>

#include "SystemWindowEventListener.h"


namespace Slab::Graphics {

    class EventTranslator {
    protected:
        Vector<Volatile<SystemWindowEventListener>> guiListeners;

    public:
        bool addGUIEventListener(const Volatile<SystemWindowEventListener> &guiEventListener);
        void clear();

    };


}

#endif //STUDIOSLAB_GUIEVENTTRANSLATOR_H
