//
// Created by joao on 2/10/23.
//

#include "FlatField2DArtist.h"

#include <memory>
#include <functional>
// #include <string_view>

#include "Graphics/Graph/Graph.h"
#include "Math/Function/Maps/R2toR/Model/R2toRDiscreteFunction.h"
#include "Core/Tools/Resources.h"
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
        updateColorBar();

        program.setUniform("colormap", colorBar.getTexture()->getTextureUnit());
    }

    void FlatField2DArtist::draw(const Graph2D &graph) {
        if (func == nullptr) return;

        drawGUI();

        if(!isVisible()) return;

        if (!validTextureData)
            repopulateTextureBuffer();

        {
            textureData->bind();
            colorBar.getTexture()->bind();
            program.use();

            auto region = graph.getRegion();
            fix x = region.xMin, y = region.yMin, w = region.width(), h = region.height();

            fix xScale = 2.f / w;
            fix xTranslate = -1.0f - 2.0f * x / w;
            fix yScale = 2.f / h;
            fix yTranslate = -1.0f - 2.0f * y / h;

            glm::mat3x3 transform = {
                    xScale, 0.0f, 0.0f,
                    0.0f, yScale, 0.0f,
                    xTranslate, yTranslate, 1.0f
            };

            program.setUniform("transformMatrix", transform);

            vertexBuffer.render(GL_TRIANGLES);
        }

        if(showColorBar)
        {
            const int left = -300;
            const int vpWidth = graph.getViewport().width();
            const int vpHeight = graph.getViewport().height();
            const int cbarWidth = 100;
            const int cbarHeight = 0.6 * vpHeight;
            const int cbarTop = (vpHeight - cbarHeight) / 2;

            colorBar.setLocation({vpWidth + left,
                                  vpWidth + left + cbarWidth,
                                  vpHeight - cbarTop,
                                  vpHeight - cbarTop - cbarHeight});
            colorBar.draw(graph);
        }

    }

    void FlatField2DArtist::invalidateTextureData() { validTextureData = false; }

    void FlatField2DArtist::repopulateTextureBuffer() {
        if(func == nullptr || validTextureData) return;

        assert(func->isDiscrete());

        auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

        auto xRes = discreteFunc.getN();
        auto yRes = discreteFunc.getM();

        discreteFunc.getSpace().syncHost();

        for (int i=0;i<xRes;++i) for (int j=0;j<yRes;++j)
            textureData->setValue(i, j, (Real32)discreteFunc.At(i, j));

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
                if(ImGui::Checkbox("Visible##", &visible))
                    setVisibility(visible);


                if (func->isDiscrete()) {
                    auto &dFunc = *dynamic_cast<const R2toR::DiscreteFunction *>(func.get());
                    ImGui::Text("%ix%i elements", dFunc.getN(), dFunc.getM());
                }

                {
                    {
                        ImGui::Text("σ := sign(ϕ)");
                        ImGui::Text("ϕ ↦ σ ln(|ϕ|×ε + 1) / ln(ϕₛₐₜ×ε + 1)");
                    }

                    bool antiAlias = textureData->getAntiAlias();
                    if (ImGui::Checkbox("Anti-alias display##", &antiAlias))
                        textureData->setAntiAlias(antiAlias);

                    if (ImGui::Button("Compute ϕₛₐₜ")) {
                        adjustScale();
                    }

                    {
                        auto cMap_saturationValue_f = (float) cMap_saturationValue;
                        auto cMap_sqrtKappa_f = (float) sqrt(cMap_kappaArg);

                        if (ImGui::DragFloat("ϕₛₐₜ",
                                             &cMap_saturationValue_f,
                                             cMap_saturationValue_f*5e-3f,
                                             1.e-5f,
                                             1.e5f,
                                             "%.2e"))
                        {
                            cMap_saturationValue = cMap_saturationValue_f;

                            updateColorBar();
                        }

                        if (ImGui::DragFloat("√κ", &cMap_sqrtKappa_f, (float) cMap_sqrtKappa_f * 5e-3f,
                                             1e-10, 1e10, "%.1e")) {
                            cMap_kappaArg = cMap_sqrtKappa_f*cMap_sqrtKappa_f;

                            updateColorBar();
                        }
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
                        updateColorBar();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("RGB -> BGR")) {
                        cMap = cMap.bgr();
                        updateColorBar();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Inverse")) {
                        cMap = cMap.inverse();
                        updateColorBar();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Reverse")) {
                        cMap = cMap.reverse();
                        updateColorBar();
                    }

                    ImGui::Checkbox("Show colorbar##", &showColorBar);
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

        field_min = function->min();
        field_max = function->max();

        if(Common::areEqual(field_min, field_max)) field_max += 0.1;

        cMap_saturationValue = Common::max(abs(field_max), abs(field_min));

        if(field_min >= 0.0) symmetricMaxMin = false;
        else symmetricMaxMin = true;

        program.setUniform("symmetric", (GLboolean ) symmetricMaxMin);
        program.setUniform("phi_sat", (GLfloat) cMap_saturationValue);

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
            program.setUniform("field_data", textureData->getTextureUnit());

            updateColorBar();
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

            fix Lx = xMax_f-xMin_f;

            vertexBuffer.clear();
            GLuint indices[6] = {0, 1, 2, 0, 2, 3};

            fix n=0;
            for(int i=-n; i<=n; ++i) {
                fix Δx = Lx*(float)i;
                FlatFieldVertex vertices[4] = {
                        {xMin_f+Δx, yMin_f, si, ti},
                        {xMax_f+Δx, yMin_f, sf, ti},
                        {xMax_f+Δx, yMax_f, sf, tf},
                        {xMin_f+Δx, yMax_f, si, tf}};

                vertexBuffer.pushBack(vertices, 4, indices, 6);
            }
        }

        invalidateTextureData();
        repopulateTextureBuffer();
    }

    auto FlatField2DArtist::getFunction() const -> R2toR::Function::ConstPtr { return func; }

    void FlatField2DArtist::setColorMap(const Styles::ColorMap &colorMap) {
        cMap = colorMap;

        symmetricMaxMin = cMap.getType() == Styles::ColorMap::Divergent;
        program.setUniform("symmetric", true);

        updateColorBar();
    }

    void FlatField2DArtist::updateColorBar() {
        fix min = field_min;
        fix max = field_max;

        auto &kappa = cMap_kappaArg;

        std::function<Real(Real)> g⁻¹;


        if(symmetricMaxMin) {
            g⁻¹ = [min, max, &kappa](Real x) {
                x = x*(max-min)+min;
                const auto s = SIGN(x);

                return kappa*(exp(s*x) - 1);
            };
        }
        g⁻¹ = [](Real x) {
            return x;
        };


        colorBar.setInverseScalingFunction(g⁻¹);

        colorBar.setColorMap(cMap);

        program.setUniform("phi_sat", (float)cMap_saturationValue);
        program.setUniform("kappa", (float)cMap_kappaArg);
        program.setUniform("symmetric", symmetricMaxMin);

    }

    void FlatField2DArtist::set_xPeriodicOn() { textureData->set_sPeriodicOn(); }

    FlatField2DArtist::FieldDataTexturePtr FlatField2DArtist::getFieldTextureData() const { return textureData; }

    void FlatField2DArtist::adjustScale() {
        field_min = func->min();
        field_max = func->max();

        cMap_saturationValue = Common::max(abs(field_max), abs(field_min));

        updateColorBar();
    }

} // Graphics