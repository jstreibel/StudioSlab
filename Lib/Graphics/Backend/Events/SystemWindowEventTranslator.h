//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_SYSTEMWINDOWEVENTTRANSLATOR_H
#define STUDIOSLAB_SYSTEMWINDOWEVENTTRANSLATOR_H

#include <Utils/List.h>

#include "SystemWindowEventListener.h"


namespace Slab::Graphics {

    class FEventTranslator {
        friend class FPlatformWindow;
    protected:
        List<TVolatile<FPlatformWindowEventListener>> SysWinListeners;

    public:
        FEventTranslator();
        virtual ~FEventTranslator() = default;


        bool AddGUIEventListener(const TVolatile<FPlatformWindowEventListener> &GuiEventListener);
        void clear();

    };


}

#endif //STUDIOSLAB_SYSTEMWINDOWEVENTTRANSLATOR_H
