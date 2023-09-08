//
// Created by joao on 01/06/23.
//

#include "Resources.h"
#include "Core/Tools/Log.h"

Str Resources::fontFileName(int index) {
    if(index > fonts.size()-1) {
        Log::Error() << "Resources: no font for (index+1) " << index+1 << ". Total fonts is " << fonts.size();
        throw "No such font";
    }

    return FontsFolder + fonts[index];
}
