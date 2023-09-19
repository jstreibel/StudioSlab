//
// Created by joao on 19/09/23.
//

#include "Texture2D.h"
#include "Core/Tools/Log.h"
#include "Utils.h"

namespace OpenGL {
    Texture2D::Texture2D(GLsizei w, GLsizei h) : Texture(Target::Texture_2D) , w(w), h(h) {
        GLint maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        if(w>maxTextureSize || h>maxTextureSize) {
            Log::Error() << "Requested texture size " << w << "x" << h
                         << " too big: max texture size allowed is " << maxTextureSize << "x" << maxTextureSize
                         << Log::Flush;

            return;
        }

        bind();

        Log::Critical() << "OpenGL::Texture is reserving GPU texture space to upload "
                        << w << "x" << h << " pixels to." << Log::Flush;

        auto pixelDataFormat = GL_RGBA;
        auto pixelDataType = GL_UNSIGNED_BYTE;
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, pixelDataFormat, pixelDataType, nullptr);

        if(!OpenGLUtils::checkGLErrors("reserve " + ToStr(w) + "x" + ToStr(h) + " GPU texture pixels"))
            Log::Success() << "OpenGL::Texture reserved " << w << "x" << h << " GPU texture pixels." << Log::Flush;

    }
} // OpenGL