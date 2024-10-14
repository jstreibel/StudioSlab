//
// Created by joao on 2/10/23.
//

#include "R2toRFunctionArtist.h"

#include <memory>
#include <utility>

#include "Graphics/Plot2D/Plot2DWindow.h"
#include "3rdParty/ImGui.h"
#include "Graphics/Plot2D/Artists/Painters/HeightmapShadingPainter.h"


namespace Slab::Graphics {

    R2toRFunctionArtist::R2toRFunctionArtist()
    : painters({{"Colored", New<Colormap1DPainter>()},
                {"Heightmap", New<HeightmapShadingPainter>()}})
    , textureKontraptions()
    {
        current_painter = painters["Colored"];
        updateMinMax();
    }

    bool R2toRFunctionArtist::draw(const Plot2DWindow &graph) {
        if (func == nullptr) return true;

        if (!validTextureData)
            repopulateTextureBuffer();

        current_painter->use();
        current_painter->setRegion(graph.getRegion().getRect());

        for(auto &thingy : textureKontraptions->blocks)
        {
            current_painter->setFieldDataTexture(thingy->texture);
            thingy->vertexBuffer->render(GL_TRIANGLES);
        }

        return true;
    }

    void R2toRFunctionArtist::invalidateTextureData() { validTextureData = false; }

    void R2toRFunctionArtist::repopulateTextureBuffer() {
        if(func == nullptr || validTextureData) return;

        assert(func->isDiscrete());

        auto &discreteFunc = dynamic_cast<const R2toR::NumericFunction&>(*func);

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
        ImGui::BeginChild((myName).c_str(), {0,24*TEXT_BASE_HEIGHT}, true/*, ImGuiWindowFlags_AlwaysAutoResize*/);

        if (func->isDiscrete()) {
            auto &dFunc = *dynamic_cast<const R2toR::NumericFunction *>(func.get());
            ImGui::Text("%ix%i elements", dFunc.getN(), dFunc.getM());
        }

        {
            if (ImGui::Button("Anti-alias toggle##")) {
                anti_alias = !anti_alias;
                for (auto &thingy: textureKontraptions->blocks)
                    thingy->texture->setAntiAlias(anti_alias);
            }
        }

        ImGui::BeginGroup();
        for(const auto& painter : painters) {
            auto name = UniqueName(painter.first);
            if (ImGui::RadioButton(name.c_str(), current_painter==painter.second))
                current_painter = painter.second;
        }
        ImGui::EndGroup();

        ImGui::Separator();
        current_painter->drawGUI();
        updateMinMax();

        ImGui::EndChild();
    }

    void R2toRFunctionArtist::setFunction(R2toR::Function_constptr funky, const Unit &unit) {
        funcUnit = unit;

        if(func == funky)
            return;

        func = std::move(funky);

        updateMinMax(true);

        if(!func->isDiscrete()) NOT_IMPLEMENTED

        auto &discreteFunc = dynamic_cast<const R2toR::NumericFunction&>(*func);

        {
            auto 𝒟 = discreteFunc.getDomain();
            region = {𝒟.xMin, 𝒟.xMax, 𝒟.yMin, 𝒟.yMax};
        }

        auto x_res = discreteFunc.getN();
        auto y_res = discreteFunc.getM();

        textureKontraptions = New<FieldTextureKontraption>(x_res, y_res, region);

        invalidateTextureData();
        repopulateTextureBuffer();
    }

    auto R2toRFunctionArtist::getFunction() const -> R2toR::Function_constptr { return func; }

    void R2toRFunctionArtist::set_xPeriodicOn() {
        for(auto &block : textureKontraptions->blocks) block->texture->set_sPeriodicOn();
    }

    void R2toRFunctionArtist::setLabel(Str label) {
        for(const auto& painter : painters) painter.second->labelUpdateEvent(label);

        Artist::setLabel(label);
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

    void R2toRFunctionArtist::updateMinMax(bool force) {
        if(func== nullptr) return;

        if(current_painter->dirtyMinMax() || force) current_painter->setMinMax(func->min(), func->max());
    }

    auto
    R2toRFunctionArtist::getFieldTextureKontraption() const
    -> Pointer<FieldTextureKontraption> { return textureKontraptions; }

    void
    R2toRFunctionArtist::setPainter(Pointer<R2toRPainter> dPainter) {
        current_painter = std::move(dPainter);
    }

    auto
    R2toRFunctionArtist::getPainter()
    -> Pointer<R2toRPainter> { return current_painter; }


} // Graphics