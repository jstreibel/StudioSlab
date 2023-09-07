//
// Created by joao on 28/08/23.
//

#include "Writer.h"

#include "Utils/EncodingUtils.h"

#include "3rdParty/freetype-gl/freetype-gl.h"
#include "3rdParty/freetype-gl/font-manager.h"

#include "Core/Graphics/OpenGL/rougier/shader.h"
#include "Core/Tools/Log.h"

// #define DEBUG_SHOW_ATLAS_TEXTURE

const Str shaderDir = "/home/joao/Developer/StudioSlab/Lib/Core/Graphics/OpenGL/rougier/shaders/";

typedef struct {
    float x, y, z;    // position
    float s, t;       // texture
    float r, g, b, a; // color
} vertex_t;

Core::Graphics::Writer::Writer(const Str& fontFile, float ptSize) {
    atlas   = texture_atlas_new(512, 512, 1);
    font    = texture_font_new_from_file(atlas, ptSize, fontFile.c_str());
    buffer  = vertex_buffer_new("vertex:3f,tex_coord:2f,color:4f");

    Styles::Color white = {1, 1, 1, 1};
    setBufferText(glyphsToLoad, {0, 0}, white);

    glGenTextures(1, &atlas->id);
    glBindTexture(GL_TEXTURE_2D, atlas->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (GLsizei)atlas->width, (GLsizei)atlas->height,
                 0, GL_RED, GL_UNSIGNED_BYTE, atlas->data);

    shader = shader_load((shaderDir + "v3f-t2f-c4f.vert").c_str(),
                         (shaderDir + "v3f-t2f-c4f.frag").c_str());

    mat4_set_identity(&projection);
    mat4_set_identity(&model);
    mat4_set_identity(&view);

    Log::Info() << "Writer with font '" << fontFile << "' with size " << ptSize << "pts instantiated" << Log::Flush;
}

Core::Graphics::Writer::~Writer() {
    glDeleteTextures( 1, &atlas->id );
    atlas->id = 0;
    texture_atlas_delete( atlas );

    vertex_buffer_delete(buffer);

    texture_font_delete(font);
}

void Core::Graphics::Writer::setBufferText(const Str &textStr, Point2D pen, Styles::Color color) {
    vertex_buffer_clear(buffer);

    size_t i;
    float r = color.r, g = color.g, b = color.b, a = color.a;
    auto text = textStr.c_str();
    for( i = 0; i < strlen(text); i += utf8_characterByteSize(text+i))
    {
        auto code_point = text + i;

        texture_glyph_t *glyph = texture_font_get_glyph( font, code_point );
        if( glyph != nullptr )
        {
            float kerning =  0.0f;
            if( i > 0)
            {
                kerning = texture_glyph_get_kerning( glyph, text + i - 1 );
            }
            pen.x += kerning;
            const int x0  = (int)( pen.x + glyph->offset_x );
            const int y0  = (int)( pen.y + glyph->offset_y );
            const int x1  = (int)( x0 + glyph->width );
            const int y1  = (int)( y0 - glyph->height );
            const float s0 = glyph->s0;
            const float t0 = glyph->t0;
            const float s1 = glyph->s1;
            const float t1 = glyph->t1;
            GLuint indices[6] = {0,1,2, 0,2,3};
            vertex_t vertices[4] = { { (float)x0,(float)y0,0,  s0,t0,  r,g,b,a },
                                     { (float)x0,(float)y1,0,  s0,t1,  r,g,b,a },
                                     { (float)x1,(float)y1,0,  s1,t1,  r,g,b,a },
                                     { (float)x1,(float)y0,0,  s1,t0,  r,g,b,a } };
            vertex_buffer_push_back( buffer, vertices, 4, indices, 6 );
            pen.x += glyph->advance_x;
        }
    }
}

void Core::Graphics::Writer::drawBuffer() const {
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glBindTexture(GL_TEXTURE_2D, atlas->id);

#ifndef DEBUG_SHOW_ATLAS_TEXTURE
    {
        glUseProgram(shader);
        {
            glUniform1i(glGetUniformLocation(shader, "texture"),
                        0);
            glUniformMatrix4fv(glGetUniformLocation(shader, "model"),
                               1, 0, model.data);
            glUniformMatrix4fv(glGetUniformLocation(shader, "view"),
                               1, 0, view.data);
            glUniformMatrix4fv(glGetUniformLocation(shader, "projection"),
                               1, 0, projection.data);

            vertex_buffer_render(buffer, GL_TRIANGLES);
        }
        glUseProgram(0);
    }
#else
    {
        glEnable(GL_TEXTURE_2D);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glColor3d(1,1,1);
        fix p =.95;
        fix smin = -.1;
        fix tmin = -.1;
        fix smax = 1.1;
        fix tmax = 1.1;
        glBegin(GL_QUADS);
        glTexCoord2d(smin, tmin); glVertex2d(-p, p);
        glTexCoord2d(smax, tmin); glVertex2d( p, p);
        glTexCoord2d(smax, tmax); glVertex2d( p,-p);
        glTexCoord2d(smin, tmax); glVertex2d(-p,-p);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glBegin(GL_LINE_LOOP);
        glVertex2d(-p, p);
        glVertex2d( p, p);
        glVertex2d( p,-p);
        glVertex2d(-p,-p);
        glEnd();
    }
#endif

}

void Core::Graphics::Writer::write(const Str &text, Point2D pen, Styles::Color color) {
    setBufferText(text, pen, color);

    drawBuffer();
}

Real Core::Graphics::Writer::getFontHeightInPixels() const { return font->height; }

void Core::Graphics::Writer::reshape(int w, int h) { mat4_set_orthographic( &projection, 0, (float)w,  0, (float)h, -1, 1); }




