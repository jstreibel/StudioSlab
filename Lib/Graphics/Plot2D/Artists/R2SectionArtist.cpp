//
// Created by joao on 22/05/24.
//

#include "R2SectionArtist.h"
#include "Graphics/OpenGL/LegacyGL/FunctionRenderer.h"

#include "Graphics/Plot2D/Plot2DWindow.h"

#include <utility>
#include "3rdParty/ImGui.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/Plot2D/Util/PlotStyleGUI.h"

namespace Slab::Graphics {
    void R2SectionArtist::setFunction(R2toR::Function_constptr func)  { function2D = std::move(func); }

    void R2SectionArtist::addSection(const RtoR2::ParametricCurve_ptr& section, const TPointer<PlotStyle>& style, const Str& name) {
        sections.emplace_back(section, style, name);
    }

    Vector<R2SectionArtist::SectionData> &R2SectionArtist::getSections() {
        return sections;
    }

    bool R2SectionArtist::Draw(const FPlot2DWindow &PlotWindow) {
        if(function2D == nullptr) return true;

        for (const auto& section_data : sections) {
            if(!section_data.visible) continue;

            auto section = section_data.section;
            auto style = section_data.style;
            auto name = section_data.name;

            OpenGL::Legacy::PushLegacyMode();
            OpenGL::Legacy::SetupOrtho(PlotWindow.GetRegion().getRect());
            OpenGL::Legacy::FunctionRenderere::RenderSection(*function2D, *section, *style, samples);
            OpenGL::Legacy::RestoreFromLegacyMode();

            PlotWindow.RequireLabelOverlay(name, style);
        }

        return true;

    }

    void R2SectionArtist::setSamples(Resolution res) { samples = res; }

    auto R2SectionArtist::getFunction() const -> R2toR::Function_constptr {
        return function2D;
    }

    bool R2SectionArtist::HasGUI() { return true; }

    void R2SectionArtist::DrawGUI() {
        auto samples_int = (int)samples;
        if(ImGui::SliderInt(UniqueName("Samples").c_str(), &samples_int, 100, 25000)
         | ImGui::DragInt(UniqueName("Samples (fine)").c_str(), &samples_int, 1)) {
            samples = static_cast<Resolution>(samples_int);
        }

        int i=0;
        for(auto &section_data : sections) {
            auto style = section_data.style;
            ImGui::SeparatorText((Str("Section ") + ToStr(++i)).c_str());

            ImGui::Checkbox((UniqueName("Visible")+section_data.name).c_str(), &section_data.visible);

            DrawPlotStyleGUI(*(section_data.style), section_data.name);
        }
    }

    void R2SectionArtist::clearSections() {
        sections.clear();
    }

    const RectR &R2SectionArtist::GetRegion() {
        if(sections.empty() || function2D==nullptr) return FArtist::GetRegion();

        auto s_min = sections[0].section->get_sMin();
        auto s_max = sections[0].section->get_sMax();

        auto val_min =  Infinity;
        auto val_max = -Infinity;

        for(const auto& section : sections) {
            for (int i = 0; i < samples; ++i) {
                auto s = s_min + (i / (DevFloat) samples) * (s_max - s_min);

                auto r = (*section.section)(s);
                auto val = (*function2D)(r);

                if(val<val_min) val_min=val;
                if(val>val_max) val_max=val;
            }
        }

        region = {s_min, s_max, val_min, val_max};

        return FArtist::GetRegion();
    }


} // Graphics