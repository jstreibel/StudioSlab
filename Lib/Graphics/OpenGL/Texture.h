//
// Created by joao on 17/09/23.
//

#ifndef STUDIOSLAB_TEXTURE_H
#define STUDIOSLAB_TEXTURE_H

#include "OpenGL.h"
#include "PixelData.h"
#include "TextureUtils.h"
#include "Graphics/Styles/Colors.h"

namespace Slab::Graphics::OpenGL {

    class FTexture {
        GLuint m_Handle=0;
        Target m_Target;
        GLenum m_TextureUnit;

        const InternalFormat m_Internalformat;

        std::shared_ptr<PixelData> p_PixelData;

    protected:
        GLuint GetHandle() const;


    public:
        explicit FTexture(Target target,
                         InternalFormat format,
                         GLenum textureUnit = GL_TEXTURE0);

        virtual ~FTexture();

        static void EnableTextures();
        static void DisableTextures();
        void Activate() const;
        static void Deactivate() ;
        void Bind() const;

        auto GetTarget() const -> Target;
        auto GetInternalFormat() const -> InternalFormat;

        void SetData(const PixelData &data);
        auto GetData() const -> const PixelData&;

        void SetBorderColor(FColor color) const;

        int GetTextureUnit() const;
        GLuint GetGLtextureUnit() const;

        void Diagnose() const;
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE_H
