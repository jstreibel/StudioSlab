//
// Created by joao on 4/10/23.
//

#include "R2toRFunctionActor.h"

#include "Core/Tools/Resources.h"

#include "Graphics/Plot3D/Scene3DWindow.h"
#include "3rdParty/ImGui.h"

#include <array>
#include <cmath>
#include <utility>

namespace Slab::Graphics {

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
    fix intensity = 2.f;
    fix a = intensity*(.5f);
    fix b = intensity*(1.f - a);
    LightData light1 = { 1,  0, 0 + zLight, a, b, 0};
    LightData light2 = { 0,  1, 0 + zLight, 0, a, b};
    LightData light3 = {-M_SQRT1_2, -M_SQRT1_2, 0 + zLight, b, 0, a};

    void GenerateXYPLane(OpenGL::VertexBuffer &buffer, int N, int M,
                         float width, float height);

    R2toRFunctionActor::R2toRFunctionActor(R2toR::FNumericFunction_constptr function)
    : func(std::move(function))
    , gridMetadata(R2toRFunctionActor::GridMetadata::FromNumericFunction(func))
    , program(Core::Resources::ShadersFolder + "FieldShading.vert",
              Core::Resources::ShadersFolder + "FieldShading.frag")
    , vertexBuffer("position:2f,texcoord:2f")
    , texture((GLsizei)gridMetadata.gridN, (GLsizei)gridMetadata.gridM)
    {
        gridMetadata.generateXYPlane(vertexBuffer);

        rebuildTextureData();

        texture.setAntiAliasOff();
        texture.setSWrap(OpenGL::ClampToEdge);
        texture.setTWrap(OpenGL::ClampToEdge);

        program.SetUniform("field", texture.getTextureUnit());

        program.SetUniform("light1_position", light1.pos());
        program.SetUniform("light2_position", light2.pos());
        program.SetUniform("light3_position", light3.pos());
        program.SetUniform("light1_color", light1.color());
        program.SetUniform("light2_color", light2.color());
        program.SetUniform("light3_color", light3.color());

        const int gridSubdivs = 8;
        program.SetUniform("gridSubdivs", gridSubdivs);

        program.SetUniform("scale", 1.f);

        fix gridN = gridMetadata.gridN;
        fix gridM = gridMetadata.gridM;
        program.SetUniform("texelSize", Real2D(1./(DevFloat)gridM, 1./(DevFloat)gridN));
    }

    void R2toRFunctionActor::draw(const Scene3DWindow &graph3D) {
        texture.Bind();

        auto camera = graph3D.getCamera();
        auto view = camera.getViewTransform();
        auto proj = camera.getProjection();
        auto model = glm::mat4(1.f);

        program.SetUniform("eye", camera.pos);

        program.SetUniform("modelview", view*model);
        program.SetUniform("projection", proj);

        vertexBuffer.Render(GL_TRIANGLES);
    }

    void R2toRFunctionActor::setAmbientLight(Color color) { program.SetUniform("amb", color.array()); }

    void R2toRFunctionActor::setGridSubdivs(int n) { program.SetUniform("gridSubdivs", n); }

    bool R2toRFunctionActor::hasGUI() {
        return true;
    }

    void R2toRFunctionActor::drawGUI() {
        if(ImGui::Button("Rebuild texture data")) {
            rebuildTextureData();
        }

        static float scale = 1.0;
        if(ImGui::DragFloat("scale", &scale, scale*2.5e-2f, 1e-4f, 1e4f, "%.2e"))
            program.SetUniform("scale", scale);

        static float gloomPowBase = 50.0;
        static float gloomMultiplier = 1.0;
        if(ImGui::DragFloat("gloom base", &gloomPowBase, gloomPowBase*1.e-2f, 0.1f, 1.e4f))
            program.SetUniform("gloomPowBase", gloomPowBase);
        if(ImGui::DragFloat("gloom multiplier", &gloomMultiplier, 0.1f, 0.1f, 10.f))
            program.SetUniform("gloomMultiplier", gloomMultiplier);

        static int gridSubdivs = 8;
        if(ImGui::DragInt("Grid subdivs", &gridSubdivs, .1f, 0, 8))
            setGridSubdivs(powf32x(2, gridSubdivs));

        const char *items[] = {"Color", "Normals", "Gloom"};
        static int current = 0;
        if(ImGui::Combo("Shading", &current, items, 3))
            program.SetUniform("shading", current);
    }

    void R2toRFunctionActor::rebuildTextureData() {
        fix gridN = gridMetadata.gridN;
        fix gridM = gridMetadata.gridM;

        for(auto i=0; i<gridN; ++i) for(auto j=0; j<gridM; ++j)
                texture.setValue(i, j, func->At(i, j));

        texture.upload();
    }

    void R2toRFunctionActor::GridMetadata::generateXYPlane(OpenGL::VertexBuffer &buffer) const {
        Vector<GLuint> indices;
        Vector<Field2DVertex> vertices;

        fix M = gridN;
        fix N = gridM;
        fix width = wSpace;
        fix height = hSpace;
        fix xMin = xMinSpace;
        fix yMin = yMinSpace;

        fix xNormFactor = 1/float(M-1);
        fix yNormFactor = 1/float(N-1);

        for (int i = 0; i < N; ++i) {
            fix t = (float)i*yNormFactor;
            fix y = t*height + yMin;
            for (int j = 0; j < M; ++j) {
                fix s = (float)j*xNormFactor;
                fix x = s*width + xMin;

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

    R2toRFunctionActor::GridMetadata
    R2toRFunctionActor::GridMetadata::FromNumericFunction(const R2toR::FNumericFunction_constptr &func) {
        fix D = func->getDomain();

        R2toRFunctionActor::GridMetadata gridMetadata;
        gridMetadata.gridN = (int)func->getN();
        gridMetadata.gridM = (int)func->getM();
        gridMetadata.xMinSpace = (float)D.xMin;
        gridMetadata.yMinSpace = (float)D.yMin;
        gridMetadata.wSpace = (float)(D.xMax - D.xMin);
        gridMetadata.hSpace = (float)(D.yMax - D.yMin);

        return gridMetadata;
    }

} // Graphics