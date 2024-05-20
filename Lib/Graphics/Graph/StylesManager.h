//
// Created by joao on 30/09/23.
//

#ifndef STUDIOSLAB_STYLESMANAGER_H
#define STUDIOSLAB_STYLESMANAGER_H

#include "Utils/Singleton.h"

#include "Core/Backend/Events/GUIEventListener.h"
#include "GraphTheme.h"

namespace Graphics {

    class StylesManager : public Singleton<StylesManager>, public Core::GUIEventListener {
    protected:
        friend Singleton<StylesManager>;

        StylesManager();

    public:
        static GraphTheme_ptr GetCurrent();

        bool notifyRender() override;
    };

} // Graphics

#endif //STUDIOSLAB_STYLESMANAGER_H
