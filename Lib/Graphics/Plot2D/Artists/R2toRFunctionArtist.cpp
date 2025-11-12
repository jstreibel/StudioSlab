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
    : textureKontraptions()
    , painters({{"Colormap", New<Colormap1DPainter>()},
        {"Heightmap", New<HeightmapShadingPainter>()}})
    {
        current_painter = painters["Colormap"];
        updateMinMax();
    }

    bool R2toRFunctionArtist::Draw(const FPlot2DWindow &graph) {
        if (func == nullptr) return true;

        if(dataIsMutable) invalidateTextureData();

        if (!validTextureData) repopulateTextureBuffer();

        current_painter->Use();
        current_painter->setRegion(graph.GetRegion().getRect());

        for(auto &thingy : textureKontraptions->blocks)
        {
            current_painter->setFieldDataTexture(thingy->texture);
            thingy->vertexBuffer->Render(GL_TRIANGLES);
        }

        return true;
    }

    void R2toRFunctionArtist::invalidateTextureData() { validTextureData = false; }

    void R2toRFunctionArtist::repopulateTextureBuffer() {
        if(func == nullptr || validTextureData) return;

        assert(func->isDiscrete());

        auto &discreteFunc = dynamic_cast<const R2toR::FNumericFunction&>(*func);

        discreteFunc.getSpace().syncHost();

        fix max_size = OpenGL::FTexture2D::GetMaxTextureSize();

        fix cols = textureKontraptions->n;
        fix rows = textureKontraptions->m;
        for(auto col=0; col<cols; ++col) {
            fix x_offset = col*max_size;
            for(auto row=0; row<rows; ++row) {
                fix y_offset = row*max_size;

                auto current_image = textureKontraptions->getBlock(col, row);
                auto texture_data = current_image->texture;

                fix tex_xres = texture_data->GetWidth();
                fix tex_yres = texture_data->GetHeight();

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

    bool R2toRFunctionArtist::HasGUI() { return true; }

    void R2toRFunctionArtist::DrawGUI() {
        if(func == nullptr) {
            ImGui::TextColored(ImVec4{1,0,0,1}, "nullptr ℝ²↦ℝ function");
            return;
        }

        const auto myName = GetLabel();

        const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
        ImGui::BeginChild((myName).c_str(), {0,24*TEXT_BASE_HEIGHT}, true/*, ImGuiWindowFlags_AlwaysAutoResize*/);

        if (func->isDiscrete()) {
            auto &dFunc = *dynamic_cast<const R2toR::FNumericFunction *>(func.get());
            ImGui::Text("%ix%i elements", dFunc.getN(), dFunc.getM());
        }

        {
            ImGui::Checkbox(UniqueName("Mutable data").c_str(), &dataIsMutable);
            if(ImGui::Button(UniqueName("Refresh data").c_str()))
                invalidateTextureData();
        }

        {
            if (ImGui::Button("Anti-alias toggle##")) {
                anti_alias = !anti_alias;
                for (auto &thingy: textureKontraptions->blocks)
                    thingy->texture->SetAntiAlias(anti_alias);
            }
        }

        ImGui::BeginGroup();
        for(const auto&[raw_name, painter] : painters) {
            auto name = UniqueName(raw_name);
            if (ImGui::RadioButton(name.c_str(), current_painter==painter))
                current_painter = painter;
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


        auto &discreteFunc = dynamic_cast<const R2toR::FNumericFunction&>(*func);

        // 𝒟ℴ𝓂𝒶𝒾𝓃
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

    void R2toRFunctionArtist::set_xPeriodicOn() const {
        for(const auto &block : textureKontraptions->blocks)
            block->texture->Set_sPeriodicOn();
    }

    void R2toRFunctionArtist::SetLabel(const Str label) {
        for(const auto& painter : painters) painter.second->labelUpdateEvent(label);

        FArtist::SetLabel(label);
    }

    Str R2toRFunctionArtist::GetXHairInfo(const Point2D &coords) const {
        if(func == nullptr) return {};

        fix r = Real2D{coords.x, coords.y};
        assert(func->domainContainsPoint(r));

        auto &discreteFunc = dynamic_cast<const R2toR::FNumericFunction &>(*func);

        fix xRes = discreteFunc.getN();
        fix yRes = discreteFunc.getM();

        fix domain = discreteFunc.getDomain();

        fix hPixelSizeInTexCoord = 1. / xRes;
        fix vPixelSizeInTexCoord = 1. / yRes;

        fix hTexturePixelSizeInSpaceCoord = hPixelSizeInTexCoord * domain.getLx();
        fix vTexturePixelSizeInSpaceCoord = vPixelSizeInTexCoord * domain.getLy();

        fix rMin = Real2D{coords.x + .5 * hTexturePixelSizeInSpaceCoord,
                          coords.y + .5 * vTexturePixelSizeInSpaceCoord};
        fix rMax = Real2D{coords.x - .5 * hTexturePixelSizeInSpaceCoord,
                          coords.y - .5 * vTexturePixelSizeInSpaceCoord};

        Str info = GetLabel() + " = ";
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

    void R2toRFunctionArtist::updateMinMax(const bool force) const {
        if(func== nullptr) return;

        if(current_painter->dirtyMinMax() || force) current_painter->setMinMax(func->min(), func->max());
    }

    auto
    R2toRFunctionArtist::getFieldTextureKontraption() const
    -> TPointer<FieldTextureKontraption> { return textureKontraptions; }

    void
    R2toRFunctionArtist::setPainter(TPointer<R2toRPainter> dPainter) {
        current_painter = std::move(dPainter);
    }

    auto
    R2toRFunctionArtist::getPainter()
    -> TPointer<R2toRPainter> { return current_painter; }

    auto R2toRFunctionArtist::getPainter(const Str &name) -> TPointer<R2toRPainter> {
        return painters[name];
    }

    void R2toRFunctionArtist::setDataMutable(const bool flag) {
        dataIsMutable = flag;
    }


} // Graphics