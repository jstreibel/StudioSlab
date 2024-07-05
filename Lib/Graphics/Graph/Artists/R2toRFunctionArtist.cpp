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
#include "3rdParty/ImGui.h"

namespace Slab::Graphics {



    R2toRFunctionArtist::R2toRFunctionArtist()
    : program(Resources::ShadersFolder+"FlatField.vert", Resources::ShadersFolder+"FlatField.frag")
    , textureKontraptions()
    , colorBar(New<OpenGL::ColorBarArtist>())
    {
    }

    bool R2toRFunctionArtist::draw(const PlottingWindow &graph) {
        if (func == nullptr) return true;

        if (!validTextureData)
            repopulateTextureBuffer();

        colorBar->getTexture()->bind();
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

        for(auto &thingy : textureKontraptions->blocks)
        {
            auto texture_data  = thingy->texture;
            texture_data->bind();
            program.setUniform("field_data", texture_data->getTextureUnit());
            program.setUniform("transformMatrix", transform);

            thingy->vertexBuffer->render(GL_TRIANGLES);
        }

        return true;
    }

    void R2toRFunctionArtist::invalidateTextureData() { validTextureData = false; }

    void R2toRFunctionArtist::repopulateTextureBuffer() {
        if(func == nullptr || validTextureData) return;

        assert(func->isDiscrete());

        auto &discreteFunc = dynamic_cast<const R2toR::NumericFunction&>(*func);

        auto x_res = discreteFunc.getN();
        auto y_res = discreteFunc.getM();

        discreteFunc.getSpace().syncHost();

        fix max_size = OpenGL::Texture2D::GetMaxTextureSize();

        fix cols = textureKontraptions->n;
        fix rows = textureKontraptions->m;
        for(auto col=0; col<cols; ++col) {
            fix x_offset = col*max_size;
            for(auto row=0; row<rows; ++row) {
                fix y_offset = row*max_size;

                auto current_image = textureKontraptions->getBlock(col, row);
                auto texture_data = current_image->texture;


                fix tex_xres = texture_data->getWidth();
                fix tex_yres = texture_data->getHeight();

                for (int tex_i=0;tex_i<tex_xres;++tex_i) for (int tex_j=0;tex_j<tex_yres;++tex_j) {
                    fix i = x_offset + tex_i;
                    fix j = y_offset + tex_j;

                    texture_data->setValue(tex_i, tex_j, (Real32) discreteFunc.At(i, j));
                }

                texture_data->upload();
            }
        }

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

            if (ImGui::Checkbox("Anti-alias display##", &anti_alias))
                for(auto &thingy : textureKontraptions->blocks)
                    thingy->texture->setAntiAlias(anti_alias);



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
                    setColorMap(ColorMaps[selectedItem]->clone());
                item_last_idx = item_current_idx;
                ImGui::EndCombo();
            }

            ImGui::Text("ColorMap operations:");
            if (ImGui::Button("RGB->BRG")) {
                *cMap = cMap->brg();
                updateColorBar();
            }
            ImGui::SameLine();
            if (ImGui::Button("RGB->BGR")) {
                *cMap = cMap->bgr();
                updateColorBar();
            }
            if (ImGui::Button("Inv")) {
                *cMap = cMap->inverse();
                updateColorBar();
            }
            ImGui::SameLine();
            if (ImGui::Button("Rev")) {
                *cMap = cMap->reverse();
                updateColorBar();
            }

            ImGui::Checkbox("Show colorbar##", &showColorBar);
        }

        ImGui::EndChild();
    }

    void R2toRFunctionArtist::setFunction(R2toR::Function_constptr functidon, const Unit &unit) {
        if(func == functidon){
            funcUnit = unit;
            return;
        }

        bool firstTime = func == nullptr;

        func = std::move(functidon);
        funcUnit = unit;

        flagMinMaxAsDirty();
        updateMinMax();

        if(Common::AreEqual(field_min, field_max)) field_max += 0.1;

        cMap_saturationValue = Common::max(abs(field_max), abs(field_min));

        if(field_min >= 0.0) symmetricMaxMin = false;
        else symmetricMaxMin = true;

        program.setUniform("symmetric", (GLboolean ) symmetricMaxMin);
        program.setUniform("phi_sat", (GLfloat) cMap_saturationValue);
        colorBar->setSymmetric(symmetricMaxMin);
        colorBar->setPhiSaturation(cMap_saturationValue);

        if(!func->isDiscrete()) NOT_IMPLEMENTED

        auto &discreteFunc = dynamic_cast<const R2toR::NumericFunction&>(*func);

        {
            auto 𝒟 = discreteFunc.getDomain();
            region = {𝒟.xMin, 𝒟.xMax, 𝒟.yMin, 𝒟.yMax};
        }

        auto xRes = discreteFunc.getN();
        auto yRes = discreteFunc.getM();

        if(firstTime
        || textureKontraptions->get_xres() != xRes
        || textureKontraptions->get_yres() != yRes)
            textureKontraptions = New<FieldTextureKontraption>(xRes, yRes, region);

        if(cMap == nullptr) {
            if (symmetricMaxMin) setColorMap(ColorMaps["BrBG"]->clone());
            else                 setColorMap(ColorMaps["blues"]->clone());
        }

        updateColorBar();

        invalidateTextureData();
        repopulateTextureBuffer();
    }

    auto R2toRFunctionArtist::getFunction() const -> R2toR::Function_constptr { return func; }

    void R2toRFunctionArtist::setColorMap(const Pointer<ColorMap> &colorMap) {
        cMap = colorMap;

        symmetricMaxMin = cMap->getType() == ColorMap::Divergent;
        program.setUniform("symmetric", true);

        updateColorBar();

    }

    void R2toRFunctionArtist::updateColorBar() {
        if(func == nullptr) return;

        updateMinMax();

        std::function<Real(Real)> g⁻¹;

        if(symmetricMaxMin) {

            g⁻¹ = [this](Real x) {
                x = x*(field_max-field_min)+field_min;
                const auto s = SIGN(x);

                return cMap_kappaArg*(exp(s*x) - 1);
            };
        } else {
            g⁻¹ = [](Real x) {
                return x;
            };
        }

        colorBar->setInverseScalingFunction(g⁻¹);

        colorBar->setColorMap(cMap);

        program.setUniform("phi_sat", (float)cMap_saturationValue);
        program.setUniform("kappa", (float)cMap_kappaArg);
        program.setUniform("symmetric", symmetricMaxMin);
        program.setUniform("colormap", colorBar->getTexture()->getTextureUnit());

        colorBar->setPhiMin(field_min);
        colorBar->setPhiMax(field_max);
        colorBar->setPhiSaturation(cMap_saturationValue);
        colorBar->setKappa(cMap_kappaArg);
        colorBar->setSymmetric(symmetricMaxMin);
        colorBar->setMode(OpenGL::ColorBarMode::ValuesInSatRangeOnly);

    }

    void R2toRFunctionArtist::set_xPeriodicOn() {
        for(auto &block : textureKontraptions->blocks) block->texture->set_sPeriodicOn();
    }

    void R2toRFunctionArtist::setLabel(Str label) {
        colorBar->setLabel(label + " (colorbar)");
        Artist::setLabel(label);
    }
    // R2toRFunctionArtist::FieldDataTexturePtr R2toRFunctionArtist::getFieldTextureData() const { return textureData; }

    void R2toRFunctionArtist::adjustScale() {
        if(func == nullptr) return;

        updateMinMax();

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

    void R2toRFunctionArtist::flagMinMaxAsDirty() { dirty_minmax = true; }

    void R2toRFunctionArtist::updateMinMax() {
        if(!dirty_minmax || func == nullptr) return;

        field_min = func->min();
        field_max = func->max();

        dirty_minmax = false;
    }

    auto R2toRFunctionArtist::getFieldTextureKontraption() const -> Pointer<FieldTextureKontraption> {
        return textureKontraptions;
    }

    auto R2toRFunctionArtist::getColorBarArtist() const -> Pointer<Graphics::OpenGL::ColorBarArtist> {
        return colorBar;
    }


} // Graphics