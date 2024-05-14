//
// Created by joao on 30/09/23.
//

#ifndef STUDIOSLAB_STYLESMANAGER_H
#define STUDIOSLAB_STYLESMANAGER_H

#include "Styles.h"

#include "Utils/Singleton.h"

#include "Core/Backend/Events/GUIEventListener.h"

namespace Math {

    class StylesManager : public Singleton<StylesManager>, public Core::GUIEventListener {
    protected:
        friend Singleton<StylesManager>;

        StylesManager();

    public:
        static Styles::ColorScheme_ptr GetCurrent();

        bool notifyRender() override;
    };

} // Math

#endif //STUDIOSLAB_STYLESMANAGER_H
