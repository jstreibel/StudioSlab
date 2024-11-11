//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_SYSTEMWINDOWEVENTTRANSLATOR_H
#define STUDIOSLAB_SYSTEMWINDOWEVENTTRANSLATOR_H

#include <vector>
#include <utility>

#include "SystemWindowEventListener.h"


namespace Slab::Graphics {

    class EventTranslator {
        friend class SystemWindow;
    protected:
        Vector<Volatile<SystemWindowEventListener>> syswin_listeners;

    public:
        EventTranslator();
        virtual ~EventTranslator() = default;


        bool addGUIEventListener(const Volatile<SystemWindowEventListener> &guiEventListener);
        void clear();

    };


}

#endif //STUDIOSLAB_SYSTEMWINDOWEVENTTRANSLATOR_H
