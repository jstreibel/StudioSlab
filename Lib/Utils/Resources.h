//
// Created by joao on 4/6/23.
//

#ifndef STUDIOSLAB_RESOURCES_H
#define STUDIOSLAB_RESOURCES_H

#include "Types.h"

namespace Resources {

    const auto Folder = std::string("/home/joao/Developer/StudioSlab/Resources/");
    const auto FontsFolder = Folder + "Fonts/";

    const StrVector fonts = {"imgui/Cousine-Regular.ttf",               //  0
                             "imgui/DroidSans.ttf",                     //  1
                             "imgui/Karla-Regular.ttf",                 //  2
                             "imgui/ProggyClean.ttf",                   //  3
                             "imgui/ProggyTiny.ttf",                    //  4
                             "imgui/Roboto-Medium.ttf",                 //  5

                             "LatinModern/lmroman10-regular.otf",       //  6
                             "LatinModern/lmromancaps10-regular.otf",   //  7

                             "Lucida/LSANS.TTF",                        //  8

                             "JuliaMono/JuliaMono-Regular.ttf",         //  9

                             "EBGaramond-VariableFont_wght.ttf",        // 10
                             "FontAwesome-Solid.ttf",                   // 11

                             "Inconsolata-Regular.ttf",                 // 12

                             "Roboto-Regular.ttf",                      // 13
                             "times-new-roman-14.ttf",                  // 14
                             "Simplex.ttf"                              // 15
                             };

    Str fontFileName(int index);

}

#endif //STUDIOSLAB_RESOURCES_H
