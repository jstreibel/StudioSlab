//
// Created by joao on 4/10/23.
//

#include "Field2DActor.h"

#include "Utils/Resources.h"

#include "Graph3D.h"
#include "imgui.h"

#include <array>

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

    fix zLight = 2.f;
    fix a = .5f;
    fix b = 1.f - a;
    LightData light1 = { 1,  0, 0 + zLight, a, b, 0};
    LightData light2 = { 0,  1, 0 + zLight, 0, a, b};
    LightData light3 = {-1, -1, 0 + zLight, b, 0, a};

    fix gridSubdivs = 1;
    fix gridN = 21;
    fix gridM = 21;
    fix xMinSpace = -10.f;
    fix yMinSpace = -10.f;
    fix wSpace = 2.f*(float)fabs(xMinSpace);
    fix hSpace = 2.f*(float)fabs(yMinSpace);

    void GenerateXYPLane(OpenGL::VertexBuffer &buffer, int N, int M,
                         float width, float height);

    Field2DActor::Field2DActor()
    : program(Resources::ShadersFolder + "FieldTestShading.vert",
              Resources::ShadersFolder + "FieldTestShading.frag")
    , vertexBuffer("position:2f,texcoord:2f")
    , texture(gridN, gridM)
    {
        GenerateXYPLane(vertexBuffer, gridN, gridM, wSpace, hSpace);

        for(auto i=0; i<gridN; ++i) for(auto j=0; j<gridM; ++j) {
            float x = 2.f*(float)M_PI*(float)j/(float)gridM + xMinSpace;
            float y = 2.f*(float)M_PI*(float)i/(float)gridN + yMinSpace;
            float r = sqrtf(x*x+y*y);

            texture.setValue(i, j, cos(r));
        }

        texture.upload();

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
        program.setUniform("dr", Real2D(wSpace/(Real)gridM, hSpace/(Real)gridN));
    }

    void Field2DActor::draw(const Graph3D &graph3D) {
        texture.bind();

        ImGui::Begin("Actor debug");
        if(ImGui::SliderFloat3("light1.pos", &light1.x, -5, 5))
            program.setUniform("light1_position", light1.pos());
        ImGui::End();

        auto proj = graph3D.getProjection();
        auto view = graph3D.getViewTransform();
        auto model = glm::mat4(1.f);

        program.setUniform("modelview", view*model);
        program.setUniform("projection", proj);

        vertexBuffer.render(GL_TRIANGLES);
    }


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