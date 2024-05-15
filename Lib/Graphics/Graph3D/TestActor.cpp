//
// Created by joao on 11/10/23.
//

#include "TestActor.h"

#include "Core/Tools/Resources.h"

#include "Graph3D.h"
#include "imgui.h"

#include <array>
#include <cmath>

namespace Graphics {

    typedef std::array<float,2> vec2;
    typedef std::array<float,3> vec3;

    struct Field2DVertex{
        float x, y;
        float s, t;
    };

    struct LightData {
        float x, y, z;
        float r, g, b;

        std::array<float, 3> pos()   const { return {x, y, z}; }
        std::array<float, 3> color() const { return {r, g, b}; }
    };

    fix gridSubdivs = 8;
    fix gridN = 64;
    fix gridM = 64;
    fix xMinSpace = -8.f;
    fix yMinSpace = -8.f;
    fix wSpace = 2.f*(float)fabs(xMinSpace);
    fix hSpace = 2.f*(float)fabs(yMinSpace);

    fix zLight = 2.f;
    fix intensity = 2.f;
    fix a = intensity*(.5f);
    fix b = intensity*(1.f - a);
    LightData light1 = { 1,  0, 0 + zLight, a, b, 0};
    LightData light2 = { 0,  1, 0 + zLight, 0, a, b};
    LightData light3 = {-M_SQRT1_2, -M_SQRT1_2, 0 + zLight, b, 0, a};

    void GenerateXYPLane(OpenGL::VertexBuffer &buffer, int N, int M,
                         float width, float height);

    TestActor::TestActor()
            : program(Resources::ShadersFolder + "FieldShading.vert",
                      Resources::ShadersFolder + "FieldShading.frag")
            , vertexBuffer("position:2f,texcoord:2f")
            , texture(gridN, gridM)
    {
        GenerateXYPLane(vertexBuffer, gridN+1, gridM+1, wSpace, hSpace);

        for(auto i=0; i<gridN; ++i) for(auto j=0; j<gridM; ++j) {
                float x = wSpace*(float)j/(float)(gridM-1) + xMinSpace;
                float y = hSpace*(float)i/(float)(gridN-1) + yMinSpace;
                float r² = x*x+y*y;
                float k = 4.0f;
                texture.setValue(i, j, cosf(2*M_PI*sqrt(r²)/k)*std::exp(-r²/(k*k)));
            }

        texture.upload();
        texture.setAntiAliasOff();
        texture.setSWrap(OpenGL::ClampToEdge);
        texture.setTWrap(OpenGL::ClampToEdge);

        program.setUniform("field", texture.getTextureUnit());

        program.setUniform("light1_position", light1.pos());
        program.setUniform("light2_position", light2.pos());
        program.setUniform("light3_position", light3.pos());
        program.setUniform("light1_color", light1.color());
        program.setUniform("light2_color", light2.color());
        program.setUniform("light3_color", light3.color());

        program.setUniform("gridSubdivs", gridSubdivs);

        program.setUniform("scale", 1.f);

        program.setUniform("texelSize", Real2D(1./(Real)gridM, 1./(Real)gridN));
    }

    void TestActor::draw(const Graph3D &graph3D) {
        texture.bind();

        auto camera = graph3D.getCamera();
        auto view = camera.getViewTransform();
        auto proj = camera.getProjection();
        auto model = glm::mat4(1.f);

        ImGui::Begin("Actor");
        static float scale = 1.0;
        if(ImGui::SliderFloat("scale", &scale, .1f, 10.f))
            program.setUniform("scale", scale);

        static float gloomPowBase = 50.0;
        static float gloomMultiplier = 1.0;
        if(ImGui::DragFloat("gloom base", &gloomPowBase, gloomPowBase*1.e-2f, 0.1f, 1.e4f))
            program.setUniform("gloomPowBase", gloomPowBase);
        if(ImGui::DragFloat("gloom multiplier", &gloomMultiplier, 0.1f, 0.1f, 10.f))
            program.setUniform("gloomMultiplier", gloomMultiplier);

        const char *items[] = {"Color", "Normals", "Gloom"};
        static int current = 0;
        if(ImGui::Combo("Shading", &current, items, 3))
            program.setUniform("shading", current);

        ImGui::End();

        program.setUniform("eye", camera.pos);

        program.setUniform("modelview", view*model);
        program.setUniform("projection", proj);

        vertexBuffer.render(GL_TRIANGLES);
    }

    void TestActor::setAmbientLight(Styles::Color color) { program.setUniform("amb", color.array()); }


    void GenerateXYPLane(OpenGL::VertexBuffer &buffer,
                         int N, int M,
                         float width, float height)
    {
        std::vector<GLuint> indices;
        std::vector<Field2DVertex> vertices;

        fix xNormFactor = 1/float(M-1);
        fix yNormFactor = 1/float(N-1);

        for (int i = 0; i < N; ++i) {
            fix t = (float)i*yNormFactor;
            fix y = t*height + yMinSpace;
            for (int j = 0; j < M; ++j) {
                fix s = (float)j*xNormFactor;
                fix x = s*width + xMinSpace;

                vertices.push_back({x, y, s, t});
            }
        }

        // Generate indices
        for (int i = 0; i < N - 1; ++i) {
            for (int j = 0; j < M - 1; ++j) {
                int topLeft = i * M + j;
                int topRight = i * M + j + 1;
                int bottomLeft = (i + 1) * M + j;
                int bottomRight = (i + 1) * M + j + 1;

                // Triangle 1
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                // Triangle 2
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        buffer.pushBack(&vertices[0], vertices.size(), &indices[0], indices.size());
    }
} // Graphics