//
// Created by joao on 4/6/23.
//

#ifndef STUDIOSLAB_RESOURCES_H
#define STUDIOSLAB_RESOURCES_H

#include "Types.h"

namespace Resources {

    const auto Folder = std::string("/home/joao/Developer/StudioSlab/Resources/");
    const auto FontsFolder = Folder + "Fonts/";

    const StrVector fonts = {"imgui/Cousine-Regular.ttf",
                             "imgui/DroidSans.ttf",
                             "imgui/Karla-Regular.ttf",
                             "imgui/ProggyClean.ttf",
                             "imgui/ProggyTiny.ttf",
                             "imgui/Roboto-Medium.ttf",

                             "EBGaramond-VariableFont_wght.ttf",
                             "FontAwesome-Solid.ttf",
                             "Inconsolata-Regular.ttf",

                             "Roboto-Regular.ttf",
                             "times-new-roman-14.ttf",};

    String fontFileName(int index);

}

#endif //STUDIOSLAB_RESOURCES_H
