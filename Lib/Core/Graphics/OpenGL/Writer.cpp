//
// Created by joao on 28/08/23.
//

#include "Writer.h"
#include "3rdParty/freetype-gl/freetype-gl.h"
#include "Core/Graphics/OpenGL/rougier/shader.h"
#include "Core/Tools/Log.h"

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

    const char *glyphsToLoad = "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z"
                               "a b c d e f g h i j k l m n o p q r s t u v w x y z"
                               "𝒜ℬ𝒞 𝒟 ℰℱ    ℐ                ℛ                "
                               "      ℯ         𝓂                        "
                               "ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ"
                               "αβγδεζηθικλμνξοπρστυφχψω"
                               "   𝜕ϵ  ϑ ϰ     ϖϱς  ϕ   "
                               "¹²³⁴⁵⁶⁷⁸⁹⁰₁₂₃₄₅₆₇₈₉₀"
                               "ᵃᵇᶜᵈᵉᶠᵍʰⁱʲᵏˡᵐⁿᵒᵖ ʳˢᵗᵘᵛʷˣʸᶻ"
                               "ₐ   ₑ  ₕᵢⱼₖₗₘₙₒₚ ᵣₛₜᵤᵥ ₓ  "
                               ""
                               "𝔄 𝔅 ℭ 𝔇 𝔈 𝔉 𝔊 ℌ ℑ 𝔍 𝔎 𝔏 𝔐 𝔑 𝔒 𝔓 𝔔 ℜ 𝔖 𝔗 𝔘 𝔙 𝔚 𝔛 𝔜 ℨ"
                               "𝔞 𝔟 𝔠 𝔡 𝔢 𝔣 𝔤 𝔥 𝔦 𝔧 𝔨 𝔩 𝔪 𝔫 𝔬 𝔭 𝔮 𝔯 𝔰 𝔱 𝔲 𝔳 𝔴 𝔵 𝔶 𝔷"
                               "𝕬 𝕭 𝕮 𝕯 𝕰 𝕱 𝕲 𝕳 𝕴 𝕵 𝕶 𝕷 𝕸 𝕹 𝕺 𝕻 𝕼 𝕽 𝕾 𝕿 𝖀 𝖁 𝖂 𝖃 𝖄 𝖅"
                               "𝖆 𝖇 𝖈 𝖉 𝖊 𝖋 𝖌 𝖍 𝖎 𝖏 𝖐 𝖑 𝖒 𝖓 𝖔 𝖕 𝖖 𝖗 𝖘 𝖙 𝖚 𝖛 𝖜 𝖝 𝖞 𝖟"
                               "                  ℑ          ℜ "
                               "0123456789|\\\"'!@#$%&*()_+-=`{[^~}]?/;:.>,<";

    Styles::Color white = {1, 1, 1, 1};
    setBufferText(glyphsToLoad, {20, 20}, white);

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
    for( i = 0; i < strlen(text); ++i )
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
    glUseProgram(0);
}

void Core::Graphics::Writer::write(const Str &text, Point2D pen, Styles::Color color) {
    setBufferText(text, pen, color);

    drawBuffer();
}

Real Core::Graphics::Writer::getFontHeightInPixels() const {
    return font->height;
}

void Core::Graphics::Writer::reshape(int w, int h) {
    mat4_set_orthographic( &projection, 0, (float)w,  0, (float)h, -1, 1);
}




