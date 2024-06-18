//
// Created by joao on 2/10/23.
//

#include "R2toRFunctionArtist.h"

#include <memory>
#include <functional>
// #include <string_view>

#include "Graphics/Graph/PlottingWindow.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Core/Tools/Resources.h"
#include "imgui.h"

namespace Slab::Graphics {

    struct FlatFieldVertex {
        float x, y;     // position
        float s, t;     // texture
    };

    R2toRFunctionArtist::R2toRFunctionArtist()
    : vertexBuffer("vertex:2f,tex_coord:2f")
    , program(Resources::ShadersFolder+"FlatField.vert", Resources::ShadersFolder+"FlatField.frag")
    , colorBar()
    {
        updateColorBar();

        program.setUniform("colormap", colorBar.getTexture()->getTextureUnit());
    }

    bool R2toRFunctionArtist::draw(const PlottingWindow &graph) {
        if (func == nullptr) return true;

        if (!validTextureData)
            repopulateTextureBuffer();

        {
            textureData->bind();
            colorBar.getTexture()->bind();
            program.use();

            auto graphRect = graph.getRegion().getRect();
            fix x = graphRect.xMin, y = graphRect.yMin, w = graphRect.width(), h = graphRect.height();

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
            const int cbarWidth = -0.35 * left; // 27.334
            const int cbarHeight = 0.96 * vpHeight;
            const int cbarTop = (vpHeight - cbarHeight) / 2;

            //               icpx         gnu
            // bench        27.334       27.992
            // compile    2min 37sec   2min 44sec

            colorBar.setLocation({vpWidth + left,
                                  vpWidth + left + cbarWidth,
                                  vpHeight - cbarTop,
                                  vpHeight - cbarTop - cbarHeight});
            colorBar.draw(graph);
        }

        return true;
    }

    void R2toRFunctionArtist::invalidateTextureData() { validTextureData = false; }

    void R2toRFunctionArtist::repopulateTextureBuffer() {
        if(func == nullptr || validTextureData) return;

        assert(func->isDiscrete());

        auto &discreteFunc = dynamic_cast<const R2toR::NumericFunction&>(*func);

        auto xRes = discreteFunc.getN();
        auto yRes = discreteFunc.getM();

        discreteFunc.getSpace().syncHost();

        for (int i=0;i<xRes;++i) for (int j=0;j<yRes;++j)
            textureData->setValue(i, j, (Real32)discreteFunc.At(i, j));

        textureData->upload();

        validTextureData = true;
    }

    bool R2toRFunctionArtist::hasGUI() { return true; }

    void R2toRFunctionArtist::drawGUI() {
        if(func == nullptr) {
            ImGui::TextColored(ImVec4{1,0,0,1}, "nullptr ℝ²↦ℝ function");
            return;
        }

        auto myName = getLabel();

        const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
        ImGui::BeginChild((myName).c_str(), {0,16*TEXT_BASE_HEIGHT}, true/*, ImGuiWindowFlags_AlwaysAutoResize*/);

        if (func->isDiscrete()) {
            auto &dFunc = *dynamic_cast<const R2toR::NumericFunction *>(func.get());
            ImGui::Text("%ix%i elements", dFunc.getN(), dFunc.getM());
        }

        {
            {
                ImGui::Text("σ := sign(ϕ)");
                ImGui::Text("μ(ϕ) = σ ln(|ϕ|/κ + 1)");
                ImGui::Text("ϕ ↦ μ(ϕ)/μ(ϕₛₐₜ)");
            }

            bool antiAlias = textureData->getAntiAlias();
            if (ImGui::Checkbox("Anti-alias display##", &antiAlias))
                textureData->setAntiAlias(antiAlias);


            {
                auto cMap_saturationValue_f = (float) cMap_saturationValue;
                auto cMap_sqrtKappa_f = (float) sqrt(cMap_kappaArg);

                if (ImGui::DragFloat("##",
                                     &cMap_saturationValue_f,
                                     cMap_saturationValue_f*5e-3f,
                                     1.e-5f,
                                     1.e5f,
                                     "%.2e"))
                {
                    cMap_saturationValue = cMap_saturationValue_f;

                    updateColorBar();
                }
                ImGui::SameLine();
                if (ImGui::Button("ϕₛₐₜ")) adjustScale();

                if (ImGui::DragFloat("√κ", &cMap_sqrtKappa_f, (float) cMap_sqrtKappa_f * 5e-3f,
                                     1e-10, 1e10, "%.1e")) {
                    cMap_kappaArg = cMap_sqrtKappa_f*cMap_sqrtKappa_f;

                    updateColorBar();
                }
            }
        }


        {
            StrVector items;
            for (const auto &cMapPair: ColorMaps)
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
                    setColorMap(ColorMaps[selectedItem]);
                item_last_idx = item_current_idx;
                ImGui::EndCombo();
            }

            ImGui::Text("ColorMap operations:");
            if (ImGui::Button("RGB->BRG")) {
                cMap = cMap.brg();
                updateColorBar();
            }
            ImGui::SameLine();
            if (ImGui::Button("RGB->BGR")) {
                cMap = cMap.bgr();
                updateColorBar();
            }
            if (ImGui::Button("Inv")) {
                cMap = cMap.inverse();
                updateColorBar();
            }
            ImGui::SameLine();
            if (ImGui::Button("Rev")) {
                cMap = cMap.reverse();
                updateColorBar();
            }

            ImGui::Checkbox("Show colorbar##", &showColorBar);
        }

        ImGui::EndChild();
    }

    void R2toRFunctionArtist::setFunction(R2toR::Function_constptr function, const Unit &unit) {
        if(func == function){
            funcUnit = unit;
            return;
        }

        auto field_min = function->min();
        auto field_max = function->max();

        if(Common::AreEqual(field_min, field_max)) field_max += 0.1;

        cMap_saturationValue = Common::max(abs(field_max), abs(field_min));

        if(field_min >= 0.0) symmetricMaxMin = false;
        else symmetricMaxMin = true;

        program.setUniform("symmetric", (GLboolean ) symmetricMaxMin);
        program.setUniform("phi_sat", (GLfloat) cMap_saturationValue);
        colorBar.setSymmetric(symmetricMaxMin);
        colorBar.setPhiSaturation(cMap_saturationValue);

        bool firstTime = func == nullptr;
        func = std::move(function);
        funcUnit = unit;

        if(!func->isDiscrete()) NOT_IMPLEMENTED

        auto &discreteFunc = dynamic_cast<const R2toR::NumericFunction&>(*func);

        {
            auto 𝒟 = discreteFunc.getDomain();
            region = {𝒟.xMin, 𝒟.xMax, 𝒟.yMin, 𝒟.yMax};
        }

        auto xRes = discreteFunc.getN();
        auto yRes = discreteFunc.getM();

        if(firstTime
           || textureData->getWidth() != xRes
           || textureData->getHeight() != yRes)
        {
            textureData = Slab::New<OpenGL::Texture2D_Real>((int) xRes, (int) yRes);
            textureData->setSWrap(Graphics::OpenGL::ClampToEdge);
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

        if(symmetricMaxMin) setColorMap(ColorMaps["BrBG"]);
        else {
            setColorMap(ColorMaps["blues"]);
        }

        invalidateTextureData();
        repopulateTextureBuffer();
    }

    auto R2toRFunctionArtist::getFunction() const -> R2toR::Function_constptr { return func; }

    void R2toRFunctionArtist::setColorMap(const ColorMap &colorMap) {
        cMap = colorMap;

        symmetricMaxMin = cMap.getType() == ColorMap::Divergent;
        program.setUniform("symmetric", true);

        updateColorBar();
    }

    void R2toRFunctionArtist::updateColorBar() {
        if(func == nullptr) return;

        fix field_min = func->min();
        fix field_max = func->max();

        auto &kappa = cMap_kappaArg;

        std::function<Real(Real)> g⁻¹;

        if(symmetricMaxMin) {
            g⁻¹ = [field_min, field_max, &kappa](Real x) {
                x = x*(field_max-field_min)+field_min;
                const auto s = SIGN(x);

                return kappa*(exp(s*x) - 1);
            };
        } else {
            g⁻¹ = [](Real x) {
                return x;
            };
        }

        colorBar.setInverseScalingFunction(g⁻¹);

        colorBar.setColorMap(cMap);

        program.setUniform("phi_sat", (float)cMap_saturationValue);
        program.setUniform("kappa", (float)cMap_kappaArg);
        program.setUniform("symmetric", symmetricMaxMin);

        colorBar.setPhiMin(field_min);
        colorBar.setPhiMax(field_max);
        colorBar.setPhiSaturation(cMap_saturationValue);
        colorBar.setKappa(cMap_kappaArg);
        colorBar.setSymmetric(symmetricMaxMin);
        colorBar.setMode(OpenGL::ColorBarMode::ValuesInSatRangeOnly);

    }

    void R2toRFunctionArtist::set_xPeriodicOn() { textureData->set_sPeriodicOn(); }

    R2toRFunctionArtist::FieldDataTexturePtr R2toRFunctionArtist::getFieldTextureData() const { return textureData; }

    void R2toRFunctionArtist::adjustScale() {
        if(func == nullptr) return;

        fix field_min = func->min();
        fix field_max = func->max();

        cMap_saturationValue = Common::max(abs(field_max), abs(field_min));

        updateColorBar();
    }

    Str R2toRFunctionArtist::getXHairInfo(const Point2D &coords) const {
        if(func == nullptr) return {};

        fix r = Real2D{coords.x, coords.y};
        assert(func->domainContainsPoint(r));

        auto &discreteFunc = dynamic_cast<const R2toR::NumericFunction &>(*func);

        auto xRes = discreteFunc.getN();
        auto yRes = discreteFunc.getM();

        auto domain = discreteFunc.getDomain();

        auto hPixelSizeInTexCoord = 1. / xRes;
        auto vPixelSizeInTexCoord = 1. / yRes;

        auto hTexturePixelSizeInSpaceCoord = hPixelSizeInTexCoord * domain.getLx();
        auto vTexturePixelSizeInSpaceCoord = vPixelSizeInTexCoord * domain.getLy();

        fix rMin = Real2D{coords.x + .5 * hTexturePixelSizeInSpaceCoord,
                          coords.y + .5 * vTexturePixelSizeInSpaceCoord};
        fix rMax = Real2D{coords.x - .5 * hTexturePixelSizeInSpaceCoord,
                          coords.y - .5 * vTexturePixelSizeInSpaceCoord};

        Str info = getLabel() + " = ";
        if (func->domainContainsPoint(r)) {
            fix val = (*func)(r);
            info += funcUnit(val, 5);
        } else if (func->domainContainsPoint(rMin)) {
            fix val = (*func)(rMin);
            info = funcUnit(val, 5);
        } else if (func->domainContainsPoint(rMax)) {
            fix val = (*func)(rMax);
            info += funcUnit(val, 5);
        }

        return info;
    }


} // Graphics