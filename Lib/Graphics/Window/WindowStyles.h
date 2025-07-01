//
// Created by joao on 02/09/2021.
//

#ifndef V_SHAPE_WINDOWSTYLES_H
#define V_SHAPE_WINDOWSTYLES_H

#include "Graphics/Styles/Colors.h"
#include "Graphics/Types2D.h"

namespace Slab::Graphics::WindowStyle {
    extern RectI DefaultWindowRect;

    extern int tiling_gap;

    extern int font_size;
    extern FColor titlebar_color;

    extern int border_size;
    extern FColor windowBorderColor_inactive;
    extern FColor windowBorderColor_active;

    extern int menu_height;

    extern FColor windowBGColor;
    extern FColor PlatformWindow_BackgroundColor;


}

#endif //V_SHAPE_WINDOWSTYLES_H
