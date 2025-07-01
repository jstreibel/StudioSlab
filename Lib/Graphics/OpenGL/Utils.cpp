//
// Created by joao on 3/31/23.
//

#include "Graphics/OpenGL/OpenGL.h"
#include <GL/glu.h>
#include "GLUTUtils.h"

#include "Utils.h"
#include "Core/Tools/Log.h"
#include <FreeImagePlus.h>

#include <cstring>
#include <iostream>
#include <utility>

#include "Shader.h"

#define CHECK_GL_ERRORS(strMark) \
    {                    \
        GLenum err;                                                 \
        while((err = glGetError()) != GL_NO_ERROR)                  \
            Log::Warning() << "OpenGL error " << err << " (" << (strMark) << "): \"" << gluErrorString(err) << "\"" << Log::Flush; \
                                                                    \
    }

namespace Slab {

    using Log = Core::Log;

    bool Graphics::OpenGL::CheckGLErrors(const Str &hint, bool raiseException) {
        bool bad = false;

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            Log::Warning() << "OpenGL error " << err << " (" << Log::FGBlue << (hint) << Log::ResetFormatting
                           << "): " << gluErrorString(err) << Log::Flush;
            bad = true;
        }

        if (bad && raiseException) throw Exception("OpenGL error");

        return bad;
    }

    void Graphics::OpenGL::DrawOrthoNormalized(RectR rect) {
        // TODO ultra-provisório
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glOrtho(0, 1, 0, 1, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glRectd(rect.xMin, rect.yMin, rect.xMax, rect.yMax);

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    }

    bool Graphics::OpenGL::OutputToPNG(FrameBuffer buffer,
                                       std::string fileName) {
        const auto w = buffer.w, h = buffer.h;
        const auto bpp = buffer.GetBitsPerPixel();
        const auto pixelData = buffer.getPixelData();
        const auto dataSize = w * h * bpp / 8;

        FIBITMAP *image = FreeImage_Allocate(w, h, bpp);
        if (!image) {
            std::cerr << "Failed to allocate image!" << std::endl;
            return false;
        }

        BYTE *bits = FreeImage_GetBits(image);
        memcpy(bits, pixelData, dataSize);


        bool success = FreeImage_Save(FIF_PNG, image, fileName.c_str(), PNG_DEFAULT);
        // bool success = FreeImage_Save(FIF_JPEG, image, fileName.c_str(), JPEG_QUALITYSUPERB);

        if (!success)
            Log::Error() << "Failed to save the image." << Log::Flush;
        else Log::Success() << "Image \"" << fileName << "\" saved successfully!" << Log::Flush;

        FreeImage_Unload(image);

        return success;
    }

    bool Graphics::OpenGL::OutputToPNG(FSlabWindow *window, std::string fileName, int width, int height) {
        // Create texture:
        GLuint texColorBuffer;
        glGenTextures(1, &texColorBuffer);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        // Create framebuffer object
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

        // Check..
        auto retVal = true;
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Log::Error() << "Outputting png: Framebuffer is not complete!" << Log::Flush;

            retVal = false;
        } else {
            const auto xOld = window->Get_x();
            const auto yOld = window->Get_y();
            const auto wOld = window->GetWidth();
            const auto hOld = window->GetHeight();

            // This offset is how frame is drawn around window.
            window->Set_x(2);
            window->Set_y(2);
            window->NotifyReshape(width-4, height-4);

            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT);

            window->OverrideSystemWindowHeight(height);

            // *********************************************
            // DANGER! Be careful with this little one.
            FPlatformWindow *DummyDanger = nullptr;
            // ReSharper disable once CppDFANullDereference
            window->ImmediateDraw(*DummyDanger);
            window->OverrideSystemWindowHeight(-1);
            // END DANGER

            window->Set_x(xOld);
            window->Set_y(yOld);
            window->NotifyReshape(wOld, hOld);

            glFlush();

            auto buffer = GLUT::getFrameBuffer(0, 0, width, height);

            OutputToPNG(buffer, std::move(fileName));
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &texColorBuffer);

        return retVal;

    }

    void Graphics::OpenGL::piccolos() {}

    Str Graphics::OpenGL::GLTypeToGLSLType(GLenum type) {
        std::map<GLenum, Str> typeMap =
                {
                        {
                                {GL_FLOAT, "float"},
                                {GL_FLOAT_VEC2, "vec2"},
                                {GL_FLOAT_VEC3, "vec3"},
                                {GL_FLOAT_VEC4, "vec4"},
                                {GL_DOUBLE, "double"},
                                {GL_DOUBLE_VEC2, "dvec2"},
                                {GL_DOUBLE_VEC3, "dvec3"},
                                {GL_DOUBLE_VEC4, "dvec4"},
                                {GL_INT, "int"},
                                {GL_INT_VEC2, "ivec2"},
                                {GL_INT_VEC3, "ivec3"},
                                {GL_INT_VEC4, "ivec4"},
                                {GL_UNSIGNED_INT, "unsigned int"},
                                {GL_UNSIGNED_INT_VEC2, "uvec2"},
                                {GL_UNSIGNED_INT_VEC3, "uvec3"},
                                {GL_UNSIGNED_INT_VEC4, "uvec4"},
                                {GL_BOOL, "bool"},
                                {GL_BOOL_VEC2, "bvec2"},
                                {GL_BOOL_VEC3, "bvec3"},
                                {GL_BOOL_VEC4, "bvec4"},
                                {GL_FLOAT_MAT2, "mat2"},
                                {GL_FLOAT_MAT3, "mat3"},
                                {GL_FLOAT_MAT4, "mat4"},
                                {GL_FLOAT_MAT2x3, "mat2x3"},
                                {GL_FLOAT_MAT2x4, "mat2x4"},
                                {GL_FLOAT_MAT3x2, "mat3x2"},
                                {GL_FLOAT_MAT3x4, "mat3x4"},
                                {GL_FLOAT_MAT4x2, "mat4x2"},
                                {GL_FLOAT_MAT4x3, "mat4x3"},
                                {GL_DOUBLE_MAT2, "dmat2"},
                                {GL_DOUBLE_MAT3, "dmat3"},
                                {GL_DOUBLE_MAT4, "dmat4"},
                                {GL_DOUBLE_MAT2x3, "dmat2x3"},
                                {GL_DOUBLE_MAT2x4, "dmat2x4"},
                                {GL_DOUBLE_MAT3x2, "dmat3x2"},
                                {GL_DOUBLE_MAT3x4, "dmat3x4"},
                                {GL_DOUBLE_MAT4x2, "dmat4x2"},
                                {GL_DOUBLE_MAT4x3, "dmat4x3"},
                                {GL_SAMPLER_1D, "sampler1D"},
                                {GL_SAMPLER_2D, "sampler2D"},
                                {GL_SAMPLER_3D, "sampler3D"},
                                {GL_SAMPLER_CUBE, "samplerCube"},
                                {GL_SAMPLER_1D_SHADOW, "sampler1DShadow"},
                                {GL_SAMPLER_2D_SHADOW, "sampler2DShadow"},
                                {GL_SAMPLER_1D_ARRAY, "sampler1DArray"},
                                {GL_SAMPLER_2D_ARRAY, "sampler2DArray"},
                                {GL_SAMPLER_1D_ARRAY_SHADOW, "sampler1DArrayShadow"},
                                {GL_SAMPLER_2D_ARRAY_SHADOW, "sampler2DArrayShadow"},
                                {GL_SAMPLER_2D_MULTISAMPLE, "sampler2DMS"},
                                {GL_SAMPLER_2D_MULTISAMPLE_ARRAY, "sampler2DMSArray"},
                                {GL_SAMPLER_CUBE_SHADOW, "samplerCubeShadow"},
                                {GL_SAMPLER_BUFFER, "samplerBuffer"},
                                {GL_SAMPLER_2D_RECT, "sampler2DRect"},
                                {GL_SAMPLER_2D_RECT_SHADOW, "sampler2DRectShadow"},
                                {GL_INT_SAMPLER_1D, "isampler1D"},
                                {GL_INT_SAMPLER_2D, "isampler2D"},
                                {GL_INT_SAMPLER_3D, "isampler3D"},
                                {GL_INT_SAMPLER_CUBE, "isamplerCube"},
                                {GL_INT_SAMPLER_1D_ARRAY, "isampler1DArray"},
                                {GL_INT_SAMPLER_2D_ARRAY, "isampler2DArray"},
                                {GL_INT_SAMPLER_2D_MULTISAMPLE, "isampler2DMS"},
                                {GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, "isampler2DMSArray"},
                                {GL_INT_SAMPLER_BUFFER, "isamplerBuffer"},
                                {GL_INT_SAMPLER_2D_RECT, "isampler2DRect"},
                                {GL_UNSIGNED_INT_SAMPLER_1D, "usampler1D"},
                                {GL_UNSIGNED_INT_SAMPLER_2D, "usampler2D"},
                                {GL_UNSIGNED_INT_SAMPLER_3D, "usampler3D"},
                                {GL_UNSIGNED_INT_SAMPLER_CUBE, "usamplerCube"},
                                {GL_UNSIGNED_INT_SAMPLER_1D_ARRAY, "usampler2DArray"},
                                {GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, "usampler2DArray"},
                                {GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE, "usampler2DMS"},
                                {GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, "usampler2DMSArray"},
                                {GL_UNSIGNED_INT_SAMPLER_BUFFER, "usamplerBuffer"},
                                {GL_UNSIGNED_INT_SAMPLER_2D_RECT, "usampler2DRect"},
                                {GL_IMAGE_1D, "image1D"},
                                {GL_IMAGE_2D, "image2D"},
                                {GL_IMAGE_3D, "image3D"},
                                {GL_IMAGE_2D_RECT, "image2DRect"},
                                {GL_IMAGE_CUBE, "imageCube"},
                                {GL_IMAGE_BUFFER, "imageBuffer"},
                                {GL_IMAGE_1D_ARRAY, "image1DArray"},
                                {GL_IMAGE_2D_ARRAY, "image2DArray"},
                                {GL_IMAGE_2D_MULTISAMPLE, "image2DMS"},
                                {GL_IMAGE_2D_MULTISAMPLE_ARRAY, "image2DMSArray"},
                                {GL_INT_IMAGE_1D, "iimage1D"},
                                {GL_INT_IMAGE_2D, "iimage2D"},
                                {GL_INT_IMAGE_3D, "iimage3D"},
                                {GL_INT_IMAGE_2D_RECT, "iimage2DRect"},
                                {GL_INT_IMAGE_CUBE, "iimageCube"},
                                {GL_INT_IMAGE_BUFFER, "iimageBuffer"},
                                {GL_INT_IMAGE_1D_ARRAY, "iimage1DArray"},
                                {GL_INT_IMAGE_2D_ARRAY, "iimage2DArray"},
                                {GL_INT_IMAGE_2D_MULTISAMPLE, "iimage2DMS"},
                                {GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY, "iimage2DMSArray"},
                                {GL_UNSIGNED_INT_IMAGE_1D, "uimage1D"},
                                {GL_UNSIGNED_INT_IMAGE_2D, "uimage2D"},
                                {GL_UNSIGNED_INT_IMAGE_3D, "uimage3D"},
                                {GL_UNSIGNED_INT_IMAGE_2D_RECT, "uimage2DRect"},
                                {GL_UNSIGNED_INT_IMAGE_CUBE, "uimageCube"},
                                {GL_UNSIGNED_INT_IMAGE_BUFFER, "uimageBuffer"},
                                {GL_UNSIGNED_INT_IMAGE_1D_ARRAY, "uimage1DArray"},
                                {GL_UNSIGNED_INT_IMAGE_2D_ARRAY, "uimage2DArray"},
                                {GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE, "uimage2DMS"},
                                {GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY, "uimage2DMSArray"},
                                {GL_UNSIGNED_INT_ATOMIC_COUNTER, "atomic_uint"}
                        }
                };

        return typeMap[type];
    }

}
