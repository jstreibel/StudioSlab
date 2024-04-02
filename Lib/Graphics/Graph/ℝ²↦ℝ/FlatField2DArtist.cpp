//
// Created by joao on 2/10/23.
//

#include "FlatField2DArtist.h"

#include <memory>
#include "Graphics/Graph/Graph.h"
#include "Maps/R2toR/Model/R2toRDiscreteFunction.h"
#include "Utils/Resources.h"
#include "imgui.h"

namespace Graphics {

    struct FlatFieldVertex {
        float x, y;     // position
        float s, t;     // texture
    };

    FlatField2DArtist::FlatField2DArtist(Str name)
    : vertexBuffer("vertex:2f,tex_coord:2f")
    , program(Resources::ShadersFolder+"FlatField.vert", Resources::ShadersFolder+"FlatField.frag")
    , name(std::move(name))
    , colorBar({50,150, 50, 750})
    {
        computeColormapTexture();

        program.setUniform("colormap", cMap_texture->getTextureUnit());
        program.setUniform("phiMin", (GLfloat)cMap_min);
        program.setUniform("phiMax", (GLfloat)cMap_max);
        program.setUniform("eps", (GLfloat)cMap_epsArg);
    }

    void FlatField2DArtist::draw(const Graph2D &graph) {
        if (func == nullptr) return;

        drawGUI();

        if(!isVisible()) return;

        if (!validTextureData)
            repopulateTextureBuffer();

        auto region = graph.getRegion();
        fix x = region.xMin, y = region.yMin, w = region.width(), h = region.height();

        fix xScale = 2.f/w;
        fix xTranslate = -1.0f - 2.0f * x / w;
        fix yScale = 2.f/h;
        fix yTranslate = -1.0f - 2.0f * y / h;

        glm::mat3x3 transform = {
                xScale        , 0.0f        , 0.0f,
                0.0f          , yScale      , 0.0f,
                xTranslate    , yTranslate  , 1.0f
        };

        textureData->bind();
        cMap_texture->bind();
        program.use();
        program.setUniform("transformMatrix", transform);

        vertexBuffer.render(GL_TRIANGLES);

        const int left = -400;
        const int vpWidth = graph.getViewport().width();
        const int vpHeight = graph.getViewport().height();
        const int cbarWidth = 150;
        const int cbarHeight = 0.8 * vpHeight;
        const int cbarTop = (vpHeight-cbarHeight)/2;

        colorBar.setLocation({vpWidth+left, vpWidth+left+cbarWidth, vpWidth-cbarTop, vpWidth-cbarTop-cbarHeight});
        colorBar.draw(graph);

    }

    void FlatField2DArtist::computeColormapTexture() {
        fix nColors = 1024;
        cMap_texture = std::make_shared<::Graphics::OpenGL::Texture1D_Color>(nColors, GL_TEXTURE1);
        cMap_texture->setWrap(::Graphics::OpenGL::ClampToEdge);

        for(auto i=0; i<nColors; ++i){
            fix s = (Real)(i-1)/(Real)(nColors-2);
            fix color = cMap.mapValue(s, 0, 1);
            cMap_texture->setColor(i, color);
        }

        cMap_texture->upload();
    }

    void FlatField2DArtist::invalidateTextureData() { validTextureData = false; }

    void FlatField2DArtist::repopulateTextureBuffer() {
        if(func == nullptr || validTextureData) return;

        assert(func->isDiscrete());

        auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

        auto xRes = discreteFunc.getN();
        auto yRes = discreteFunc.getM();

        discreteFunc.getSpace().syncHost();

        for (int i=0;i<xRes;++i) for (int j=0;j<yRes;++j) textureData->setValue(i, j, (Real32)discreteFunc.At(i, j));

        textureData->upload();

        validTextureData = true;
    }

    void FlatField2DArtist::drawGUI() {
        auto myName = Str("ℝ² ") + name + " display";

        if (ImGui::Begin("Stats")) {
            if (ImGui::CollapsingHeader(myName.c_str())) {

                const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
                ImGui::BeginChild((myName).c_str(), {0,16*TEXT_BASE_HEIGHT}, true/*, ImGuiWindowFlags_AlwaysAutoResize*/);

                auto visible = isVisible();
                if(ImGui::Checkbox("Visible", &visible))
                    setVisibility(visible);


                if (func->isDiscrete()) {
                    auto &dFunc = *dynamic_cast<const R2toR::DiscreteFunction *>(func.get());
                    ImGui::Text("%ix%i elements", dFunc.getN(), dFunc.getM());
                }

                {
                    if (logScale) {
                        ImGui::Text("σ := sign(ϕ)");
                        ImGui::Text("ϕ ↦ σ ln(|ϕ|/ε + 1)");
                    }
                    ImGui::Text("ϕ ↦ (ϕ-ϕₘᵢₙ)/Δϕ, Δϕ=ϕₘₐₓ-ϕₘᵢₙ");

                    auto min = (float) cMap_min;
                    auto max = (float) cMap_max;
                    auto eps = (float) cMap_epsArg;

                    bool antiAlias = textureData->getAntiAlias();
                    if (ImGui::Checkbox("Anti-alias display", &antiAlias))
                        textureData->setAntiAlias(antiAlias);

                    float fullWidth = ImGui::GetContentRegionAvail().x;
                    float relativeWidth = fullWidth * 0.3f;
                    ImGui::Text("ϕₘᵢₙ");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(relativeWidth);
                    if (ImGui::SliderFloat("##min", &min, -10, 0)) {
                        cMap_min = min;
                        if (symmetricMaxMin) cMap_max = -min;

                        program.setUniform("phiMin", (GLfloat)cMap_min);
                        program.setUniform("phiMax", (GLfloat)cMap_max);

                    }
                    ImGui::SameLine();
                    if (ImGui::SliderFloat("ϕₘₐₓ", &max, .001f, 10)) {
                        cMap_max = max;
                        if (symmetricMaxMin) cMap_min = -max;

                        program.setUniform("phiMin", (GLfloat)cMap_min);
                        program.setUniform("phiMax", (GLfloat)cMap_max);
                    }
                    ImGui::PopItemWidth();

                    if (ImGui::DragFloat("ε", &eps, (float)eps * 1e-2f, 1e-5, 1e3, "%.6f")) {
                        cMap_epsArg = eps;
                        program.setUniform("eps", (GLfloat)cMap_epsArg);
                    }

                    if (ImGui::Checkbox("Log scale", &logScale)) {
                        program.setUniform("useLog", (GLboolean)logScale);
                    }
                }


                {
                    StrVector items;
                    for (const auto &cMapPair: Styles::ColorMaps)
                        items.emplace_back(cMapPair.first);
                    static int item_current_idx = 0; // Here we store our selection data as an index.
                    static int item_last_idx = 0;
                    Str selectedItem;
                    const char *combo_preview_value = items[item_current_idx].c_str();  // Pass in the preview value visible before opening the combo (it could be anything)
                    if (ImGui::BeginCombo("Colormaps", combo_preview_value, 0)) {
                        for (int n = 0; n < items.size(); n++) {
                            const bool is_selected = (item_current_idx == n);
                            if (ImGui::Selectable(items[n].c_str(), is_selected)) {
                                selectedItem = items[n];
                                item_current_idx = n;
                            }

                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        if (item_last_idx != item_current_idx)
                            setColorMap(Styles::ColorMaps[selectedItem]);
                        item_last_idx = item_current_idx;
                        ImGui::EndCombo();
                    }

                    ImGui::Text("ColorMap operations:");
                    if (ImGui::Button("Permute")) {
                        cMap = cMap.permute();
                        computeColormapTexture();
                        colorBar.setTexture(getColorMapTexture());
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("RGB -> BGR")) {
                        cMap = cMap.bgr();
                        computeColormapTexture();
                        colorBar.setTexture(getColorMapTexture());
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Inverse")) {
                        cMap = cMap.inverse();
                        computeColormapTexture();
                        colorBar.setTexture(getColorMapTexture());
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Reverse")) {
                        cMap = cMap.reverse();
                        computeColormapTexture();
                        colorBar.setTexture(getColorMapTexture());
                    }
                }

                ImGui::EndChild();
            }
        }
        ImGui::End();
    }

    void FlatField2DArtist::setFunction(R2toR::Function::ConstPtr function, const Unit &unit) {
        if(func == function){
            funcUnit = unit;
            return;
        }

        cMap_min = function->min();
        cMap_max = function->max();

        if(cMap_min >= 0.0) symmetricMaxMin = false;
        else symmetricMaxMin = true;



        program.setUniform("phiMin", (GLfloat) cMap_min);
        program.setUniform("phiMax", (GLfloat) cMap_max);

        bool firstTime = func == nullptr;
        func = std::move(function);
        funcUnit = unit;

        if(!func->isDiscrete()) NOT_IMPLEMENTED

        auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

        auto xRes = discreteFunc.getN();
        auto yRes = discreteFunc.getM();

        if(firstTime
           || textureData->getWidth() != xRes
           || textureData->getHeight() != yRes)
        {
            textureData = std::make_shared<::Graphics::OpenGL::Texture2D_Real>((int) xRes, (int) yRes);
            textureData->setSWrap(::Graphics::OpenGL::ClampToEdge);
            textureData->setAntiAliasOff();
            program.setUniform("fieldData", textureData->getTextureUnit());
        }

        {
            auto domain = discreteFunc.getDomain();

            auto hPixelSizeInTexCoord = 1. / xRes;
            auto vPixelSizeInTexCoord = 1. / yRes;

            auto hTexturePixelSizeInSpaceCoord = hPixelSizeInTexCoord * domain.getLx();
            auto vTexturePixelSizeInSpaceCoord = vPixelSizeInTexCoord * domain.getLy();

            auto si = 0.0f; // - hPixelSizeInTexCoord;
            auto sf = 1.0f; // + hPixelSizeInTexCoord;
            auto ti = 0.0f; // - vPixelSizeInTexCoord;
            auto tf = 1.0f; // + vPixelSizeInTexCoord;

            fix xMin_f = (float) (-.5*hTexturePixelSizeInSpaceCoord + domain.xMin);
            fix xMax_f = (float) (+.5*hTexturePixelSizeInSpaceCoord + domain.xMax);
            fix yMin_f = (float) (-.5*vTexturePixelSizeInSpaceCoord + domain.yMin);
            fix yMax_f = (float) (+.5*vTexturePixelSizeInSpaceCoord + domain.yMax);

            vertexBuffer.clear();
            GLuint indices[6] = {0, 1, 2, 0, 2, 3};
            FlatFieldVertex vertices[4] = {
                    {xMin_f, yMin_f,   si, ti},
                    {xMax_f, yMin_f,   sf, ti},
                    {xMax_f, yMax_f,   sf, tf},
                    {xMin_f, yMax_f,   si, tf}};

            vertexBuffer.pushBack(vertices, 4, indices, 6);
        }

        invalidateTextureData();
        repopulateTextureBuffer();
    }

    auto FlatField2DArtist::getFunction() const -> R2toR::Function::ConstPtr { return func; }

    void FlatField2DArtist::setColorMap(const Styles::ColorMap &colorMap) {
        cMap = colorMap;

        symmetricMaxMin = cMap.getType() == Styles::ColorMap::Divergent;
        if(symmetricMaxMin) {
            auto max = std::max(abs(cMap_min), abs(cMap_max));
            cMap_min = -max;
            cMap_max = max;

            program.setUniform("phiMin", (GLfloat) cMap_min);
            program.setUniform("phiMax", (GLfloat) cMap_max);
        }

        computeColormapTexture();

        colorBar.setTexture(getColorMapTexture());
    }

    void FlatField2DArtist::set_xPeriodicOn() { textureData->set_sPeriodicOn(); }

    FlatField2DArtist::CMapTexturePtr FlatField2DArtist::getColorMapTexture() const {
        return cMap_texture;
    }

    FlatField2DArtist::FieldDataTexturePtr FlatField2DArtist::getFieldTextureData() const {
        return textureData;
    }

} // Graphics