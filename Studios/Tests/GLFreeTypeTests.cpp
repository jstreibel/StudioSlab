//
// Created by joao on 27/08/23.
//

#include <GL/glew.h>
#include "GLFreeTypeTests.h"

#include "Utils/Resources.h"
#include "3rdParty/glfreetype/TextRenderer.hpp"
#include "3rdParty/freetype-gl/freetype-gl.h"
#include "Core/Tools/Log.h"
#include "3rdParty/freetype-gl/vertex-buffer.h"
#include "Core/Graphics/OpenGL/rougier/shader.h"

const Str shaderDir = "/home/joao/Developer/StudioSlab/Studios/Tests/shaders/";

// ------------------------------------------------------- typedef & struct ---
typedef struct {
    float x, y, z;    // position
    float s, t;       // texture
    float r, g, b, a; // color
} vertex_t;

void add_text_to_buffer( vertex_buffer_t * buffer, texture_font_t * font,
               const char * text, const vec4 * color, vec2 * pen );

GLFreeTypeTests::GLFreeTypeTests() {
    // Text to be printed
    const char *text = "A Quick Brown Fox Jumps Over The Lazy Dog 0123456789";
    // Texture atlas to store individual glyphs
    atlas = texture_atlas_new(512, 512, 1);
    // Build a new texture font from its description and size
    texture_font_t *font = texture_font_new_from_file(atlas, 28, Resources::fontFileName(6).c_str());
    // Build a new vertex buffer (position, texture & color)
    buffer = vertex_buffer_new("vertex:3f,tex_coord:2f,color:4f");
    // Where to start printing on screen
    vec2 pen = {20, 20};
    // Text color
    vec4 white = {1, 1, 1, 1};
    // Add text to the buffer
    add_text_to_buffer(buffer, font, text, &white, &pen);

    texture_font_delete(font);

    glGenTextures(1, &atlas->id);
    glBindTexture(GL_TEXTURE_2D, atlas->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas->width, atlas->height,
                 0, GL_RED, GL_UNSIGNED_BYTE, atlas->data);


    shader = shader_load((shaderDir + "v3f-t2f-c4f.vert").c_str(),
                         (shaderDir + "v3f-t2f-c4f.frag").c_str());

    mat4_set_identity(&projection);
    mat4_set_identity(&model);
    mat4_set_identity(&view);
}

GLFreeTypeTests::~GLFreeTypeTests() {
    glDeleteTextures( 1, &atlas->id );
    atlas->id = 0;
    texture_atlas_delete( atlas );
}

void GLFreeTypeTests::draw() {
    if(true) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    Window::draw();

    /*
    glMatrixMode(GL_MODELVIEW);
    fix ratio = Real(w)/h;
    glScaled(1/ratio, 1, 1);
    static Real angle = 0.0;
    glRotated(angle, 0, 0, 1);
    angle+=0.1;


    {
        // glColor3d(.6, .6, .7);
        glBegin(GL_LINES);
        glVertex2d(-0.1, .0);
        glVertex2d(0.1, .0);
        glVertex2d(.0, -0.1);
        glVertex2d(.0, 0.1);
        glEnd();
    }

    {
        glfreetype::print(fontData, 300, 300, "Hello, world :)");
    }
     */

    {
        // glDisable(GL_LINE_SMOOTH);
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glUseProgram( shader );
        {
            glUniform1i( glGetUniformLocation( shader, "texture" ),
                         0 );
            glUniformMatrix4fv( glGetUniformLocation( shader, "model" ),
                                1, 0, model.data);
            glUniformMatrix4fv( glGetUniformLocation( shader, "view" ),
                                1, 0, view.data);
            glUniformMatrix4fv( glGetUniformLocation( shader, "projection" ),
                                1, 0, projection.data);

            vertex_buffer_render(buffer, GL_TRIANGLES);
        }
    }
}

bool GLFreeTypeTests::notifyScreenReshape(int newScreenWidth, int newScreenHeight) {
    Window::notifyScreenReshape(newScreenWidth, newScreenHeight);

    auto vp = getViewport();
    mat4_set_orthographic( &projection, 0, (float)vp.w(),  0, (float)vp.h(),   -1, 1);

    return true;
}


// --------------------------------------------------------------- add_text ---
void add_text_to_buffer( vertex_buffer_t * buffer, texture_font_t * font,
               const char * text, const vec4 * color, vec2 * pen )
{
    size_t i;
    float r = color->red, g = color->green, b = color->blue, a = color->alpha;
    for( i = 0; i < strlen(text); ++i )
    {
        texture_glyph_t *glyph = texture_font_get_glyph( font, text + i );
        if( glyph != NULL )
        {
            float kerning =  0.0f;
            if( i > 0)
            {
                kerning = texture_glyph_get_kerning( glyph, text + i - 1 );
            }
            pen->x += kerning;
            const int x0  = (int)( pen->x + glyph->offset_x );
            const int y0  = (int)( pen->y + glyph->offset_y );
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
            pen->x += glyph->advance_x;
        }
    }
}