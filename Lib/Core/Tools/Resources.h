//
// Created by joao on 4/6/23.
//

#ifndef STUDIOSLAB_RESOURCES_H
#define STUDIOSLAB_RESOURCES_H

#include "Utils/Types.h"
#include "Utils/String.h"

namespace Slab::Core::Resources {

    const auto Folder = Str("/home/joao/Developer/StudioSlab/Resources/");

    const auto NanoTeXFolder = Folder + "NanoTeX/";
    const auto FontsFolder = Folder + "Fonts/";
    const auto ShadersFolder = Folder + "Shaders/";
    const auto IconsFolder = Folder + "Icons/";

    const auto SystemFolder = "/usr/share/fonts/";

    namespace BuiltinFonts {
        inline auto CousineRegular() { return FontsFolder + "imgui/Cousine-Regular.ttf"; }
        inline auto DroidSans() { return FontsFolder + "imgui/DroidSans.ttf"; }
        inline auto KarlaRegular() { return FontsFolder + "imgui/Karla-Regular.ttf"; }
        inline auto ProggyClean() { return FontsFolder + "imgui/ProggyClean.ttf"; }
        inline auto ProggyTiny() { return FontsFolder + "imgui/ProggyTiny.ttf"; }
        inline auto RobotoMedium() { return FontsFolder + "imgui/Roboto-Medium.ttf"; }
        inline auto LatinModernMath() { return FontsFolder + "LatinModern/latinmodern-math.otf"; }        //  6 (math symbols confirmed)
        inline auto LatinModern10_Regular() { return FontsFolder + "LatinModern/lmroman10-regular.otf"; }
        inline auto LatinModernCaps10_Regular() { return FontsFolder + "LatinModern/lmromancaps10-regular.otf"; }
        inline auto LucidaSans() { return FontsFolder + "Lucida/LSANS.TTF"; }
        inline auto JuliaMono_Regular() { return FontsFolder + "JuliaMono/JuliaMono-Regular.ttf"; }       // 10 (math symbols confirmed)
        inline auto EBGaramond_VariableFontWeight() { return FontsFolder + "EBGaramond-VariableFont_wght.ttf"; }
        inline auto FontAwesome_Solid() { return FontsFolder + "FontAwesome-Solid.ttf"; }
        inline auto Inconsolata_Regular() { return FontsFolder + "Inconsolata-Regular.ttf"; }
        inline auto RobotoRegular() {  return FontsFolder + "Roboto-Regular.ttf"; }
        inline auto TimesNewRoman14() { return FontsFolder + "times-new-roman-14.ttf"; }
        inline auto Simplex() { return FontsFolder +  "Simplex.ttf"; }
        inline auto DejaVuSans() { return FontsFolder + "DejaVu/DejaVuSans.ttf"; }                    // 17 (math symbols confirmed)
        inline auto NewComputerModern10_Regular() { return FontsFolder + "NewComputerModern/NewCM10-Regular.otf"; }   // 18 (math symbols confirmed) }
        inline auto NewComputerModern10_Italic() { return FontsFolder + "NewComputerModern/NewCM10-Italic.otf"; }    // 19 (math symbols confirmed) }
    };

    const StrVector ExportedFonts = {
                              "imgui/Cousine-Regular.ttf"                //  0
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

    Str GetIndexedFontFileName(int Index);

}

#endif //STUDIOSLAB_RESOURCES_H
