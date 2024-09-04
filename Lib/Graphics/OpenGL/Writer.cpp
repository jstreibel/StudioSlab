//
// Created by joao on 28/08/23.
//

#include "Writer.h"

#include "Utils/EncodingUtils.h"

#include "Graphics/OpenGL/rougier/shader.h"
#include "Core/Tools/Log.h"
#include "Utils.h"
#include "Core/Tools/Resources.h"
// #define DEBUG_SHOW_ATLAS_TEXTURE

#include <string>
#include <map>
#include <regex>


namespace Slab {

    Str parse_special(const Str& input) {
        // Define a regex to find the sequences starting with ^
        std::regex sequence_regex(R"(\^.)");
        std::string output;

        // Iterator for the regex matches
        std::sregex_iterator current_match(input.begin(), input.end(), sequence_regex);
        std::sregex_iterator last_match; // end iterator

        // Variable to track the last position in the input string
        std::size_t last_pos = 0;

        while (current_match != last_match) {
            // Get the match position
            std::smatch match = *current_match;
            std::size_t match_pos = match.position();

            // Append the part of the input string before the match
            output += input.substr(last_pos, match_pos - last_pos);

            // Get the matched sequence
            std::string key = match.str();

            // Perform the substitution if the key exists in the map
            auto it = char_map.find(key);
            if (it != char_map.end()) {
                output += it->second; // Append the substitution
            } else {
                output += key; // Append the original sequence if no substitution found
            }

            // Update last position
            last_pos = match_pos + match.length();

            // Move to the next match
            current_match++;
        }

        // Append the rest of the input string after the last match
        output += input.substr(last_pos);

        return output;
    }

    Str parse_text(Str input) {
        // Define the regular expression for the delimiters
        //std::regex delimiter(R"(\{|\})");
        std::regex delimiter(R"($)");

        // Tokenize the string based on the delimiters
        std::sregex_token_iterator iter(input.begin(), input.end(), delimiter, -1);
        std::sregex_token_iterator end;

        Str text;
        bool is_within = false;
        while (iter != end) {
            if(is_within) text += parse_special(*iter++);
            else text += *iter++;

            is_within = !is_within;
        }

        return text;
    }

    const Str shaderDir = Core::Resources::ShadersFolder + "rougier/";

    typedef struct {
        float x, y, z;    // position
        float s, t;       // texture
        float r, g, b, a; // color
    } vertex_t;

    Graphics::Writer::Writer(const Str &fontFile, float ptSize)
            : vertexBuffer("vertex:3f,tex_coord:2f,color:4f"),
              program(shaderDir + "v3f-t2f-c4f.vert", shaderDir + "v3f-t2f-c4f.frag") {

        auto factor = ptSize/10;

        atlas = texture_atlas_new(factor*512, factor*512, 1);
        font = texture_font_new_from_file(atlas, ptSize, fontFile.c_str());

        Core::Log::Critical() << "Writer being instantiated. Will start generating atlas now." << Core::Log::Flush;

        Color white = {1, 1, 1, 1};
        setBufferText(glyphsToLoad, {0, 0}, white);

        glGenTextures(1, &atlas->id);
        glBindTexture(GL_TEXTURE_2D, atlas->id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (GLsizei) atlas->width, (GLsizei) atlas->height,
                     0, GL_RED, GL_UNSIGNED_BYTE, atlas->data);

        mat4_set_identity(&projection);
        mat4_set_identity(&model);
        mat4_set_identity(&view);

        Core::Log::Success() << "Writer with font '" << fontFile << "' with size " << ptSize << "pts instantiated."
                       << Core::Log::Flush;
    }

    Graphics::Writer::~Writer() {
        glDeleteTextures(1, &atlas->id);
        atlas->id = 0;
        texture_atlas_delete(atlas);

        // "delete vertexBuffer;"

        Str name = font->filename;
        try {
            texture_font_delete(font); // TODO: problematik
        } catch(...) {
            Core::Log::Warning() << "Deletion of font '" << name << "' did not go smooth." << Core::Log::Flush;
        }
        font = nullptr;
    }

    void Graphics::Writer::setBufferText(const Str &textStr, Point2D pen, Color color, bool vertical) {
        vertexBuffer.clear();

        size_t i;
        float r = color.r, g = color.g, b = color.b, a = color.a;
        auto text = textStr.c_str();
        for (i = 0; i < strlen(text); i += utf8_characterByteSize(text + i)) {
            auto code_point = text + i;

            texture_glyph_t *glyph = texture_font_get_glyph(font, code_point);

            if(glyph == nullptr){
                Core::Log::Error() << "Graphics::Writer could not load glyph '" << code_point << "' "
                                   << "for font '" << font->filename << "'." << Core::Log::Flush;
            }
            else // if (glyph != nullptr)
            {
                float kerning = 0.0f;
                if (i > 0)
                    kerning = texture_glyph_get_kerning(glyph, text + i - 1);

                if(vertical){
                    pen.y += kerning;

                    const int x0 = (int) (pen.x - glyph->offset_y);
                    const int y0 = (int) (pen.y + glyph->offset_x + glyph->width);
                    const int x1 = (int) (x0 + glyph->height);
                    const int y1 = (int) (y0 - glyph->width);
                    const float s0 = glyph->s0;
                    const float t0 = glyph->t0;
                    const float s1 = glyph->s1;
                    const float t1 = glyph->t1;

                    GLuint indices[6] = {0, 1, 2, 0, 2, 3};
                    vertex_t vertices[4] = {{(float) x0, (float) y0, 0, s1, t0, r, g, b, a},
                                            {(float) x0, (float) y1, 0, s0, t0, r, g, b, a},
                                            {(float) x1, (float) y1, 0, s0, t1, r, g, b, a},
                                            {(float) x1, (float) y0, 0, s1, t1, r, g, b, a}};

                    vertexBuffer.pushBack(vertices, 4, indices, 6);

                    pen.y += glyph->advance_x;
                }
                else {
                    pen.x += kerning;

                    const int x0 = (int) (pen.x + glyph->offset_x);
                    const int y0 = (int) (pen.y + glyph->offset_y);
                    const int x1 = (int) (x0 + glyph->width);
                    const int y1 = (int) (y0 - glyph->height);
                    const float s0 = glyph->s0;
                    const float t0 = glyph->t0;
                    const float s1 = glyph->s1;
                    const float t1 = glyph->t1;

                    GLuint indices[6] = {0, 1, 2, 0, 2, 3};
                    vertex_t vertices[4] = {{(float) x0, (float) y0, 0, s0, t0, r, g, b, a},
                                            {(float) x0, (float) y1, 0, s0, t1, r, g, b, a},
                                            {(float) x1, (float) y1, 0, s1, t1, r, g, b, a},
                                            {(float) x1, (float) y0, 0, s1, t0, r, g, b, a}};

                    vertexBuffer.pushBack(vertices, 4, indices, 6);

                    pen.x += glyph->advance_x;
                }
            }
        }
    }

    void Graphics::Writer::drawBuffer() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        fix textureUnit = GL_TEXTURE0;
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, atlas->id);

        program.use();
        program.setUniform("texture", textureUnit - GL_TEXTURE0);
        program.setUniform4x4("model", model.data);
        program.setUniform4x4("view", view.data);
        program.setUniform4x4("projection", projection.data);

        vertexBuffer.render(GL_TRIANGLES);

    }

    void Graphics::Writer::write(const Str &text, Point2D pen, Color color, bool vertical) {
        setBufferText(parse_text(text), pen, color, vertical);
        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " (0)");

        drawBuffer();
        OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " (1)");
    }

    Real Graphics::Writer::getFontHeightInPixels() const { return font->height; }

    void Graphics::Writer::reshape(int w, int h) {
        mat4_set_orthographic(&projection, 0, (float) w, 0, (float) h, -1, 1);
    }


}