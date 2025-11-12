//
// Created by joao on 7/25/24.
//

#include "3rdParty/ImGui.h"
#include "Core/Tools/Resources.h"
#include "Colormap1DPainter.h"
#include "Math/Toolset/NativeFunctions.h"

#include <utility>



namespace Slab::Graphics {

    Colormap1DPainter::Colormap1DPainter()
    : R2toRPainter(Core::Resources::ShadersFolder+"FlatField.frag")
    {
        setKappa(kappa);
    }

    void Colormap1DPainter::drawGUI() {

        ImGui::Text("σ := sign(ϕ)");
        ImGui::Text("μ(ϕ) = σ ln(|ϕ|/κ + 1)");
        ImGui::Text("ϕ ↦ μ(ϕ)/μ(ϕₛₐₜ)");

        {
            if (fix speed = saturation_value*5e-3f;
                ImGui::DragFloat("##", &saturation_value, speed, 1.e-5f, 1.e5f, "%.2e"))
                setSaturation(saturation_value);
            ImGui::SameLine();
            if (ImGui::Button("ϕₛₐₜ")) dirty_minmax = true;

            auto sqrt_kappa = sqrtf(kappa);
            if (ImGui::DragFloat("√κ", &sqrt_kappa, sqrt_kappa * 5e-3f, 1e-10, 1e10, "%.1e")) {
                kappa = sqrt_kappa*sqrt_kappa;
                setKappa(kappa);
            }
        }

        {
            struct PairyPair {
                Str colormap_name;
                Str display_text;
            };
            Vector<PairyPair> items;
            for (const auto &cMapPair: ColorMaps) {
                auto str_category = ColorMap::CategoryToString(cMapPair.second->getType());
                auto display_text = cMapPair.first + " [" + str_category + "]";
                items.emplace_back(cMapPair.first, display_text);
            }

            static int item_current_idx = 0; // Here we store our selection data as an index.
            static int item_last_idx = 0;
            Str selectedItem;
            const char *combo_preview_value = items[item_current_idx].display_text.c_str();  // Pass in the preview value visible before opening the combo (it could be anything)
            if (ImGui::BeginCombo("Colormaps##", combo_preview_value, 0)) {
                for (int n = 0; n < items.size(); n++) {
                    const bool is_selected = (item_current_idx == n);

                    if (ImGui::Selectable(items[n].display_text.c_str(), is_selected)) {
                        selectedItem = items[n].colormap_name;
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

            auto category = Str("Category: ") + ColorMap::CategoryToString(colormap->getType());
            ImGui::Text(category.c_str(), nullptr);
            ImGui::Text("ColorMap operations:");
            if (ImGui::Button("RGB->BRG")) {
                *colormap = colormap->brg();
                updateColorbar();
            }
            ImGui::SameLine();
            if (ImGui::Button("RGB->BGR")) {
                *colormap = colormap->bgr();
                updateColorbar();
            }
            if (ImGui::Button("Inv")) {
                *colormap = colormap->inverse();
                updateColorbar();
            }
            ImGui::SameLine();
            if (ImGui::Button("Rev")) {
                *colormap = colormap->reverse();
                updateColorbar();
            }

            // ImGui::Checkbox("Show colorbar##", &showColorBar);
        }

        Painter::drawGUI();
    }

    void Colormap1DPainter::setColormapTexture(TPointer<OpenGL::Texture1D_Color> texture) {
        cmap_texture = std::move(texture);
        SetUniform("colormap", cmap_texture->GetTextureUnit());
    }

    void Colormap1DPainter::Use() const {
        if(cmap_texture) cmap_texture->Bind();

        R2toRPainter::Use();
    }

    void Colormap1DPainter::updateColorbar() {
        std::function<DevFloat(DevFloat)> g_inverse;

        if(true) {
            g_inverse = [this](DevFloat x) {
                x = x*(field_max-field_min)+field_min;
                const auto s = Math::SIGN(x);

                return kappa*(exp(s*x) - 1);
            };
        } else {
            g_inverse = [](DevFloat x) {
                return x;
            };
        }
        colorbarArtist->setInverseScalingFunction(g_inverse);

        colorbarArtist->setColorMap(colormap);

        // colorbarArtist->setPhiMin(field_min);
        // colorbarArtist->setPhiMax(field_max);
        // colorbarArtist->setPhiSaturation(saturation_value);
        // colorbarArtist->setKappa(kappa);
        // colorbarArtist->setSymmetric(symmetric_maxmin);
        colorbarArtist->setMode(OpenGL::ColorBarMode::ValuesInSatRangeOnly);

        setColormapTexture(colorbarArtist->getTexture());
    }

    bool Colormap1DPainter::dirtyMinMax() const {
        return dirty_minmax;
    }

    void Colormap1DPainter::setMinMax(DevFloat min, DevFloat max) {
        field_min = min;
        field_max = max;

        dirty_minmax = false;

        if(Common::AreEqual(field_min, field_max)) field_max += 0.1;

        setSaturation(Common::max(abs(field_max), abs(field_min)));

        colorbarArtist->setPhiMin(field_min);
        colorbarArtist->setPhiMax(field_max);

        updateColorbar();
    }

    void Colormap1DPainter::setSymmetricMaxMin(const bool is) {
        symmetric_maxmin = is;
        SetUniform("symmetric", (GLboolean ) symmetric_maxmin);
        colorbarArtist->setSymmetric(symmetric_maxmin);

        // if(symmetric_maxmin) setEpsilon(1.1f/(float)colorBar->getSamples()) else
        setEpsilon(.0);
    }

    void Colormap1DPainter::setColorMap(const TPointer<ColorMap> &colorMap) {
        colormap = colorMap;

        const auto type = colormap->getType();
        const bool sym = type == ColorMap::Divergent | type == ColorMap::Miscellaneous;
        setSymmetricMaxMin(sym);

        updateColorbar();
    }

    auto Colormap1DPainter::getColorBarArtist() -> TPointer<OpenGL::ColorBarArtist> {
        return colorbarArtist;
    }

    void Colormap1DPainter::setKappa(DevFloat) const {
        this->SetUniform("kappa", kappa);
        colorbarArtist->setKappa(kappa);
    }

    void Colormap1DPainter::setSaturation(const DevFloat sat) {
        saturation_value = static_cast<float>(sat);

        SetUniform("phi_sat", saturation_value);
        colorbarArtist->setPhiSaturation(saturation_value);
    }

    void Colormap1DPainter::setEpsilon(DevFloat eps) {

        eps_offset = static_cast<float>(eps);
        SetUniform("eps", eps_offset);
    }

    void Colormap1DPainter::labelUpdateEvent(const Str &label) {
        colorbarArtist->SetLabel("[colorbar] " + label);
    }
} // Slab::Graphics