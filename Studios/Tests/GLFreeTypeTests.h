//
// Created by joao on 27/08/23.
//

#ifndef STUDIOSLAB_GLFREETYPETESTS_H
#define STUDIOSLAB_GLFREETYPETESTS_H

#include <GL/glew.h>

#include "Base/Graphics/Window/Window.h"
#include "3rdParty/glfreetype/TextRenderer.hpp"
#include "3rdParty/freetype-gl/vertex-buffer.h"
#include "3rdParty/freetype-gl/texture-atlas.h"
#include "3rdParty/freetype-gl/demos/mat4.h"

class GLFreeTypeTests : public Window {
    glfreetype::font_data fontData;

    ftgl::vertex_buffer_t *buffer;
    GLuint shader;
    ftgl::texture_atlas_t *atlas;
    ftgl::mat4   model, view, projection;

public:
    GLFreeTypeTests();

    ~GLFreeTypeTests() override;

    void draw() override;

    bool notifyScreenReshape(int newScreenWidth, int newScreenHeight) override;
};


#endif //STUDIOSLAB_GLFREETYPETESTS_H
