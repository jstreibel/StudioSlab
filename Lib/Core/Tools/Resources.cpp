//
// Created by joao on 01/06/23.
//

#include "Resources.h"
#include "Core/Tools/Log.h"

namespace Slab::Core {

    Str Resources::GetIndexedFontFileName(int Index) {
        if (Index > ExportedFonts.size() - 1) {
            Log::Error() << "Resources: no font for (index+1) " << Index + 1 << ". Total fonts is " << ExportedFonts.size();
            throw "No such font";
        }

        return FontsFolder + ExportedFonts[Index];
    }


}