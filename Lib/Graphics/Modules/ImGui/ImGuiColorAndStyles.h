//
// Created by joao on 3/10/23.
//

#ifndef STUDIOSLAB_IMGUICOLORANDSTYLES_H
#define STUDIOSLAB_IMGUICOLORANDSTYLES_H

#include "3rdParty/ImGui.h"
#include "Utils/Arrays.h"

namespace Slab::Graphics {

    void SetColorThemeNativeLight();
    void SetColorThemeNativeDark();
    void SetColorThemeDarkRed();
    void SetStyleDark();
    void SetStyleLight();
    void SetStyleStudioSlab();
    void SetStyleScientificSlate();
    void SetStyleScientificPaper();
    void SetStyleBlueprintNight();

    void BuildImGuiThemeFontAtlas(float fontSizePixels);
    bool SetImGuiThemeFont(const Str& fontName);
    [[nodiscard]] auto ListImGuiThemeFonts() -> const Vector<Str>&;
    [[nodiscard]] auto GetCurrentImGuiThemeFont() -> Str;

}

#endif //STUDIOSLAB_IMGUICOLORANDSTYLES_H
