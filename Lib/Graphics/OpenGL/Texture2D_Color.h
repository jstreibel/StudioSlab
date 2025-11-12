//
// Created by joao on 18/08/23.
//

#ifndef STUDIOSLAB_TEXTURE2D_COLOR_H
#define STUDIOSLAB_TEXTURE2D_COLOR_H

#include "Texture2D.h"

#include "Utils/Types.h"

#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/Styles/Colors.h"
#include "Graphics/Utils/ImageLoad.h"

namespace Slab::Graphics::OpenGL {

    class FTexture2D_Color final : public FTexture2D {
        ByteData_raw data = nullptr;

    public:

        FTexture2D_Color(GLsizei width, GLsizei height);

        static TPointer<FTexture2D_Color> FromStbiImage(Image::StbiImageInfo);
        static TPointer<FTexture2D_Color> FromImageFile(const Str& FileName);

        bool SetColor(int i, int j, FColor color) const;

        bool Upload(UInt row=0, CountType nRows=0);
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE2D_COLOR_H
