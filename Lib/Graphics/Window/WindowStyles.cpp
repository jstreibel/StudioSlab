//
// Created by joao on 02/09/2021.
//

#include "WindowStyles.h"

namespace Slab::Graphics {
    int tiling_gap = 3;

    int title_bar_height = 42;
    int border_size = 3;

    RectI default_window_rect(200, 1000, 200, 750);

    int menuHeight = title_bar_height;

    // Color clearColor = {0.1, 0.1, 0.15, 1};
    // Color clearColor = {0.9, 0.9, 0.85, 1};
    Color clearColor = {0.9, 0.85, 0.85, 1};
    //Color windowBorderColor_inactive = {0.2,0.2,0.30,1};
    Color windowBorderColor_inactive = {0.,0.,0.,1};
    Color windowBorderColor_active = {0.4,0.4,0.60,1};
}