//
// Created by joao on 02/09/2021.
//

#include "WindowStyles.h"

namespace Slab::Graphics {
    RectI default_window_rect(200, 1000, 200, 750);

    int tiling_gap = 3;

    int title_bar_height = 42;
    Color titlebar_color = {232/255., 232/255., 232/255.};

    int menuHeight = title_bar_height;

    int border_size = 3;
    //Color windowBorderColor_inactive = {0.2,0.2,0.30,1};
    Color windowBorderColor_inactive = titlebar_color*0.5;
    Color windowBorderColor_active = titlebar_color;

    // Color clearColor = {0.1, 0.1, 0.15, 1};
    // Color clearColor = {0.9, 0.9, 0.85, 1};
    // Color windowBGColor = {246/255., 245/255., 244/255., 1};
    Color windowBGColor = {192/255., 181/255., 181/255., 1};


}