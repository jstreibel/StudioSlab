//
// Created by joao on 30/09/23.
//

#ifndef STUDIOSLAB_PLOTTHEMEMANAGER_H
#define STUDIOSLAB_PLOTTHEMEMANAGER_H

#include "Utils/Singleton.h"

#include "Graphics/SlabGraphics.h"

#include "Graphics/Backend/Events/SystemWindowEventListener.h"
#include "PlottingTheme.h"

namespace Slab::Graphics {

    class PlotThemeManager : public Singleton<PlotThemeManager>, public FPlatformWindowEventListener {
    protected:
        friend Singleton<PlotThemeManager>;
        friend GraphTheme_ptr LoadStyle(const Str& style);
        Str current;

        PlotThemeManager();

    public:
        static GraphTheme_ptr GetCurrent();

        bool NotifyRender(const FPlatformWindow&) override;

        static Str GetDefault();
        static bool SetTheme(const Str& theme);
        static StrVector GetThemes();
    };

} // Graphics

#endif //STUDIOSLAB_PLOTTHEMEMANAGER_H
