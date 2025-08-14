//
// Created by joao on 02/09/2021.
//

#include "WindowStyles.h"

namespace Slab::Graphics::WindowStyle {
    RectI DefaultWindowRect(200, 1000, 200, 750);

    int tiling_gap = 3;

    int font_size = 24;
    // int title_bar_height = 20;
    FColor titlebar_color = {232/255., 232/255., 232/255.};

    int menu_height = font_size*5./3;

    int BorderSize = 3;
    //Color windowBorderColor_inactive = {0.2,0.2,0.30,1};
    FColor windowBorderColor_inactive = titlebar_color*0.5;
    FColor windowBorderColor_active = titlebar_color;

    // Color clearColor = {0.1, 0.1, 0.15, 1};
    // Color clearColor = {0.9, 0.9, 0.85, 1};
    // Color windowBGColor = {246/255., 245/255., 244/255., 1};
    FColor WindowBGColor = {192/255., 181/255., 181/255., 1};

    FColor PlatformWindow_BackgroundColor = {21./255,34./255,51./255};
}