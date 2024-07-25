//
// Created by joao on 7/25/24.
//

#include "3rdParty/ImGui.h"
#include "Core/Tools/Resources.h"
#include "Colormap1DPainter.h"

#include <utility>



namespace Slab::Graphics {

    Colormap1DPainter::Colormap1DPainter()
    : Painter(Core::Resources::ShadersFolder+"FlatField.vert",
              Core::Resources::ShadersFolder+"FlatField.frag")
    {}

    void Colormap1DPainter::drawGUI() {
        {
            ImGui::Text("σ := sign(ϕ)");
            ImGui::Text("μ(ϕ) = σ ln(|ϕ|/κ + 1)");
            ImGui::Text("ϕ ↦ μ(ϕ)/μ(ϕₛₐₜ)");
        }

        {
            fix speed = saturation_value*5e-3f;
            if (ImGui::DragFloat("##", &saturation_value, speed, 1.e-5f, 1.e5f, "%.2e"))
                this->setUniform("phi_sat", saturation_value);

            ImGui::SameLine();
            if (ImGui::Button("ϕₛₐₜ"))
                adjustScale();

            auto sqrt_kappa = sqrtf(kappa);
            if (ImGui::DragFloat("√κ", &sqrt_kappa, sqrt_kappa * 5e-3f, 1e-10, 1e10, "%.1e")) {
                kappa = sqrt_kappa*sqrt_kappa;
                this->setUniform("kappa", kappa);
                updateColorbar();
            }


        }

        Painter::drawGUI();
    }

    void Colormap1DPainter::adjustScale() {
        NOT_IMPLEMENTED_CLASS_METHOD
    }

    void Colormap1DPainter::updateColorbar() {
        NOT_IMPLEMENTED_CLASS_METHOD
    }

    void Colormap1DPainter::setFieldData(Pointer<Math::R2toR::NumericFunction> data) {
        field_data = std::move(data);

    }

    void Colormap1DPainter::setColorMap(Reference<ColorMap> cmap_ref) {
        colorMap = std::move(cmap_ref);
    }
} // Slab::Graphics