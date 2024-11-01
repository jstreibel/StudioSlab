//
// Created by joao on 02/09/2021.
//

#ifndef V_SHAPE_WINDOWSTYLES_H
#define V_SHAPE_WINDOWSTYLES_H

#include "Graphics/Styles/Colors.h"
#include "Graphics/Types2D.h"

namespace Slab::Graphics::WindowStyle {
    extern RectI default_window_rect;

    extern int tiling_gap;

    extern int font_size;
    extern Color titlebar_color;

    extern int border_size;
    extern Color windowBorderColor_inactive;
    extern Color windowBorderColor_active;

    extern int menu_height;

    extern Color windowBGColor;
    extern Color sysWindowBGColor;


}

#endif //V_SHAPE_WINDOWSTYLES_H
