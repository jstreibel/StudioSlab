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

    class FPlotThemeManager : public FSingleton<FPlotThemeManager>, public FPlatformWindowEventListener {
    protected:
        friend FSingleton<FPlotThemeManager>;
        friend GraphTheme_ptr LoadStyle(const Str& style);
        Str current;

        FPlotThemeManager();

    public:
        static GraphTheme_ptr GetCurrent();

        bool NotifyRender(const FPlatformWindow&) override;

        static Str GetDefault();
        static bool SetTheme(const Str& theme);
        static StrVector GetThemes();
    };

    using PlotThemeManager [[deprecated("Use FPlotThemeManager")]] = FPlotThemeManager;

} // Graphics

#endif //STUDIOSLAB_PLOTTHEMEMANAGER_H
