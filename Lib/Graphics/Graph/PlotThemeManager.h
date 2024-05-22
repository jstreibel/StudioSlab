//
// Created by joao on 30/09/23.
//

#ifndef STUDIOSLAB_PLOTTHEMEMANAGER_H
#define STUDIOSLAB_PLOTTHEMEMANAGER_H

#include "Utils/Singleton.h"

#include "Core/Backend/Events/GUIEventListener.h"
#include "PlottingTheme.h"

namespace Graphics {

    class PlotThemeManager : public Singleton<PlotThemeManager>, public Core::GUIEventListener {
    protected:
        friend Singleton<PlotThemeManager>;

        PlotThemeManager();

    public:
        static GraphTheme_ptr GetCurrent();

        bool notifyRender() override;
    };

} // Graphics

#endif //STUDIOSLAB_PLOTTHEMEMANAGER_H
