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

    class Texture {
        GLuint handle=0;
        Target target;
        GLenum textureUnit;

        const InternalFormat format;

        std::shared_ptr<PixelData> pixelData;

    protected:
        GLuint getHandle() const;


    public:
        explicit Texture(Target target,
                         InternalFormat format,
                         GLenum textureUnit = GL_TEXTURE0);

        virtual ~Texture();

        void activate() const;
        static void deactivate() ;
        void Bind() const;

        auto getTarget() const -> Target;
        auto getInternalFormat() const -> InternalFormat;

        void setData(const PixelData &data);
        auto getData() const -> const PixelData&;

        void setBorderColor(Color color) const;

        int getTextureUnit() const;
        GLuint getGLtextureUnit() const;

        void diagnose() const;
    };

} // OpenGL

#endif //STUDIOSLAB_TEXTURE_H
