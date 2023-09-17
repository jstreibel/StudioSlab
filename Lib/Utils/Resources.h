//
// Created by joao on 4/6/23.
//

#ifndef STUDIOSLAB_RESOURCES_H
#define STUDIOSLAB_RESOURCES_H

#include "Types.h"

namespace Resources {

    const auto Folder = std::string("/home/joao/Developer/StudioSlab/Resources/");
    const auto ShadersFolder = Folder + "Shaders/";
    const auto FontsFolder = Folder + "Fonts/";

    const auto SystemFolder = "/usr/share/fonts/";

    const StrVector fonts = {"imgui/Cousine-Regular.ttf"                 //  0
                             ,"imgui/DroidSans.ttf"                      //  1
                             ,"imgui/Karla-Regular.ttf"                  //  2
                             ,"imgui/ProggyClean.ttf"                    //  3
                             ,"imgui/ProggyTiny.ttf"                     //  4
                             ,"imgui/Roboto-Medium.ttf"                  //  5

                             ,"LatinModern/latinmodern-math.otf"         //  6 (math symbols confirmed)
                             ,"LatinModern/lmroman10-regular.otf"        //  7
                             ,"LatinModern/lmromancaps10-regular.otf"    //  8

                             ,"Lucida/LSANS.TTF"                         //  9

                             ,"JuliaMono/JuliaMono-Regular.ttf"          // 10 (math symbols confirmed)

                             ,"EBGaramond-VariableFont_wght.ttf"         // 11
                             ,"FontAwesome-Solid.ttf"                    // 12

                             ,"Inconsolata-Regular.ttf"                  // 13

                             ,"Roboto-Regular.ttf"                       // 14
                             ,"times-new-roman-14.ttf"                   // 15
                             ,"Simplex.ttf"                              // 16

                             ,"DejaVu/DejaVuSans.ttf"                    // 17 (math symbols confirmed)

                             ,"NewComputerModern/NewCM10-Regular.otf"    // 18 (math symbols confirmed)
                             ,"NewComputerModern/NewCM10-Italic.otf"     // 19 (math symbols confirmed)
    };

    Str fontFileName(int index);

}

#endif //STUDIOSLAB_RESOURCES_H
