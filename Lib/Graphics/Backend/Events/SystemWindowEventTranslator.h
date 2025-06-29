//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_SYSTEMWINDOWEVENTTRANSLATOR_H
#define STUDIOSLAB_SYSTEMWINDOWEVENTTRANSLATOR_H

#include <Utils/List.h>

#include "SystemWindowEventListener.h"


namespace Slab::Graphics {

    class FEventTranslator {
        friend class SystemWindow;
    protected:
        List<Volatile<FSystemWindowEventListener>> sysWin_listeners;

    public:
        FEventTranslator();
        virtual ~FEventTranslator() = default;


        bool AddGUIEventListener(const Volatile<FSystemWindowEventListener> &guiEventListener);
        void clear();

    };


}

#endif //STUDIOSLAB_SYSTEMWINDOWEVENTTRANSLATOR_H
