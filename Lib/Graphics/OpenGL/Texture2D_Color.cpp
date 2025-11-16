//
// Created by joao on 18/08/23.
//

#include "Utils.h"
#include "Texture2D_Color.h"

#define R 0
#define G 1
#define B 2
#define A 3

#define CHECK_GL_ERRORS(count) OpenGLUtils::checkGLErrors(Str(__PRETTY_FUNCTION__) + " from " + Common::getClassName(this) + " (" + ToStr((count)) + ")");


namespace Slab::Graphics::OpenGL {

    FTexture2D_Color::FTexture2D_Color(GLsizei w, GLsizei h)
    : FTexture2D(w, h, RGBA)
    , data(static_cast<ByteData_raw>(malloc(w * h * 4)))
    {

    }

    FTexture2D_Color::Result FTexture2D_Color::FromStbiImage(Image::StbiImageInfo Image) {
        if (Image.channels !=4)
            Result::Fail("Image must have 4 channels");

        constexpr int Channels = 4;

        const auto Texture = New<FTexture2D_Color>(Image.width, Image.height);

        const auto w = Texture->GetWidth();
        const auto h = Texture->GetHeight();

        memcpy(Texture->data, Image.data.get(), w*h*Channels);

        Texture->Upload();

        return Result::Ok(Texture);
    }

    TResult<TPointer<FTexture2D_Color>> FTexture2D_Color::FromImageFile(const Str& FileName) {
        const auto data = Image::LoadImageFile(FileName);
        if (data.IsSuccess()) {
            return FromStbiImage(data.Value());
        }

        return TResult<TPointer<FTexture2D_Color>>::Fail("Failed to load image: " + FileName);
    }

    bool FTexture2D_Color::SetColor(int i, int j, FColor color) const {
        if(data == nullptr) return false;

        assert(i>=0 && i<GetWidth());
        assert(j>=0 && j<GetHeight());

        fix index = i*4 + j*GetWidth()*4;
        const ByteData_raw texel = &data[index];

        texel[R] = static_cast<Byte>(255 * color.r);
        texel[G] = static_cast<Byte>(255 * color.g);
        texel[B] = static_cast<Byte>(255 * color.b);
        texel[A] = static_cast<Byte>(255 * color.a);

        return true;
    }

    bool FTexture2D_Color::Upload(UInt row0, CountType nRows) {
        if(data == nullptr) return false;

        assert(GetInternalFormat() == GL_RGBA);

        constexpr auto dataFormat = DataFormat_RGBA;
        constexpr auto dataType = DataType_UByte;

        return UploadData(row0, nRows, dataFormat, dataType, &data[row0 * GetWidth() * 4]);
    }

} // OpenGL
