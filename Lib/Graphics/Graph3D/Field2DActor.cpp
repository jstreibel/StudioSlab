//
// Created by joao on 4/10/23.
//

#include "Field2DActor.h"

#include "Utils/Resources.h"

#include "Graph3D.h"

#include <array>

namespace Graphics {

    typedef std::array<float,2> vec2;
    typedef std::array<float,3> vec3;

    struct Field2DVertex{
        float nx, ny, nz;
        float x, y, z;
        float s, t;
    };

    fix gridN = 20;
    fix gridM = 20;
    fix xMinSpace = -1.f;
    fix yMinSpace = -1.f;
    fix wSpace = 2*abs(xMinSpace);
    fix hSpace = 2*abs(yMinSpace);

    void GenerateSurface(OpenGL::VertexBuffer &buffer,
                         int N, int M,
                         float width, float height,
                         vec3 (*surf)(vec2) = [](vec2 in){return vec3{in[0], in[1], .0f};})
    {
        std::vector<GLuint> indices;
        std::vector<Field2DVertex> vertices;

        // Generate vertices
        fix nx = .0f, ny = .0f, nz = 1.0f;

        fix xNormFactor = 1/float(M-1);
        fix yNormFactor = 1/float(N-1);

        fix xFactor = width *xNormFactor;
        fix yFactor = height*yNormFactor;

        for (int i = 0; i < N; ++i) {
            fix t =   (float)i*yFactor + yMinSpace;
            fix v = 2*(float)i*yNormFactor - 1;
            for (int j = 0; j < M; ++j) {
                fix s =   (float)j*xFactor + xMinSpace;
                fix u = 2*(float)j*xNormFactor - 1;

                auto [x, y, z] = surf({s, t});

                vertices.push_back({nx, ny, nz,
                                     x,  y,  z,
                                     u, v});
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

    Field2DActor::Field2DActor()
    : program(Resources::ShadersFolder + "FieldTestShading.vert",
              Resources::ShadersFolder + "FieldTestShading.frag")
    , vertexBuffer("normal:3f,position:3f,texcoord:2f")
    , texture(100, 100)
    {
        GenerateSurface(vertexBuffer, gridN, gridM, wSpace, hSpace);

        for(auto i=0; i<100; ++i) for(auto j=0; j<100; ++j) {
            auto x = 2*M_PI*j/100;
            auto y = 2*M_PI*i/100;
            auto r = sqrt(x*x+y*y);

            texture.setValue(i, j, sinf(r));
        }

        texture.upload();


        program.setUniform("viewMode", 0);

        program.setUniform("field", texture.getTextureUnit());

        fix zLight = 0.f;
        if (currentLightMode == White) {
            std::array<float, 3> lightPostition = {{1.f, 0.f, 0.f + zLight}};
            program.setUniform("light1_position", lightPostition);
            program.setUniform("light2_position", lightPostition);
            program.setUniform("light3_position", lightPostition);
        }
        else if (currentLightMode == Colors) {
            std::array<float, 3> light1Postition = { 1,  0, 0 + zLight};
            std::array<float, 3> light2Postition = { 0,  1, 0 + zLight};
            std::array<float, 3> light3Postition = {-1, -1, 0 + zLight};

            program.setUniform("light1_position", light1Postition);
            program.setUniform("light2_position", light2Postition);
            program.setUniform("light3_position", light3Postition);
        }

        program.setUniform("gridType", 1); // Cartesian
        program.setUniform("lightOn", 1);

        program.setUniform("gridSubdivs", 8);
        program.setUniform("showLevelLines", 0);

        program.setUniform("phiScale", 1.f);
        program.setUniform("dPhidtScale", 1.f);

        program.setUniform("dr_tex", Real2D(1./(Real)gridM, 1./(Real)gridN));
        program.setUniform("dr", Real2D(wSpace/(Real)gridM, hSpace/(Real)gridN));
    }

    void Field2DActor::draw(const Graph3D &graph3D) {
        // glEnable(GL_DEPTH_TEST);
        // glDepthFunc(GL_LESS);
        // glDepthRange(-10.f, 10.f);

        texture.bind();

        auto proj = graph3D.getProjection();
        auto view = graph3D.getViewTransform();
        auto model = glm::mat4(1.f);

        program.setUniform("modelview", view*model);
        program.setUniform("projection", proj);

        vertexBuffer.render(GL_TRIANGLES);
    }
} // Graphics