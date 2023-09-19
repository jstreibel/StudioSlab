//
// Created by joao on 4/8/23.
//

#include "FlatFieldDisplay.h"


#include "Utils/Resources.h"

#include "Core/Graphics/Styles/ColorMap.h"
#include "Core/Tools/Log.h"
#include "Core/Tools/Animator.h"

#include "Maps/R2toR/Model/R2toRDiscreteFunction.h"

#include "imgui.h"
#include "Core/Graphics/OpenGL/Texture2D_Color.h"

#include <utility>


#define drawFieldVerts(offset) \
glTexCoord2d(si, ti); glVertex2d(-.5*hTexturePixelSizeInSpaceCoord + domain.xMin+(offset), domain.yMin); \
glTexCoord2d(sf, ti); glVertex2d(-.5*hTexturePixelSizeInSpaceCoord + domain.xMax+(offset), domain.yMin); \
glTexCoord2d(sf, tf); glVertex2d(-.5*hTexturePixelSizeInSpaceCoord + domain.xMax+(offset), domain.yMax); \
glTexCoord2d(si, tf); glVertex2d(-.5*hTexturePixelSizeInSpaceCoord + domain.xMin+(offset), domain.yMax);

#define ODD true

struct FlatFieldVertex {
    float x, y;     // position
    float s, t;     // texture
};

R2toR::Graphics::FlatFieldDisplay::FlatFieldDisplay(Str title, Real phiMin, Real phiMax)
: Core::Graphics::Graph2D(-1, 1, -1, 1, std::move(title))
, cMap_min(phiMin)
, cMap_max(phiMax)
, symmetricMaxMin(Common::areEqual(phiMax,-phiMin))
, vertexBuffer("vertex:2f,tex_coord:2f")
, program(Resources::ShadersFolder+"FlatField.vert", Resources::ShadersFolder+"FlatField.frag")
{

};

void R2toR::Graphics::FlatFieldDisplay::setup(R2toR::Function::ConstPtr function) {
    func = std::move(function);

    if(!func->isDiscrete())
        throw "R2toR::Graphics::FlatFieldDisplay not-discrete function draw unimplemented";

    auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

    computeGraphRanges();

    auto xRes = discreteFunc.getN();
    auto yRes = discreteFunc.getM();

    delete texture;
    texture = new OpenGL::Texture2D_Color((int)xRes, (int)yRes);

    {
        auto domain = discreteFunc.getDomain();

        auto hPixelSizeInTexCoord = 1. / texture->getWidth();
        auto vPixelSizeInTexCoord = 1. / texture->getHeight();

        auto hTexturePixelSizeInSpaceCoord = hPixelSizeInTexCoord * domain.getLx();
        auto vTexturePixelSizeInSpaceCoord = vPixelSizeInTexCoord * domain.getLy();

        auto si = 0.0f; // - hPixelSizeInTexCoord;
        auto sf = 1.0f; // + hPixelSizeInTexCoord;
        auto ti = 0.0f; // - vPixelSizeInTexCoord;
        auto tf = 1.0f; // + vPixelSizeInTexCoord;

        fix xMin_f = -.75f; //(float) (-.5*hTexturePixelSizeInSpaceCoord + domain.xMin);
        fix xMax_f = +.75f; //(float) (-.5*hTexturePixelSizeInSpaceCoord + domain.xMax);
        fix yMin_f = -.75f; //(float) (domain.yMin);
        fix yMax_f = +.75f; //(float) (domain.yMax);

        vertexBuffer.clear();
        GLuint indices[6] = {0, 1, 2, 0, 2, 3};
        FlatFieldVertex vertices[4] = {
            {xMin_f, yMin_f, si, ti},
            {xMax_f, yMin_f, sf, ti},
            {xMax_f, yMax_f, sf, tf},
            {xMin_f, yMax_f, si, tf}};

        vertexBuffer.pushBack(vertices, 4, indices, 6);

        // Log::Debug() << "Generated vertices for '" << title << "' graph VertexBuffer. "
    }

    invalidateTextureData();
    repopulateTextureBuffer();
}

void R2toR::Graphics::FlatFieldDisplay::draw() {
    // Don't call Core::Graphics::Graph2D::draw();
    Window::draw();

    setupOrtho();

    labelingHelper.setTotalItems(countDisplayItems());
    drawFlatField();

    drawAxes();
    drawPointSets();
    drawCurves();
    drawGUI();
    drawXHair();
}

void R2toR::Graphics::FlatFieldDisplay::drawFlatField() {
    if (func == nullptr) return;

    if (!validTextureData)
        repopulateTextureBuffer();

    glEnable(GL_TEXTURE_2D);
    texture->bind();

    if(true) {
        auto region = this->getRegion();
        fix x = region.xMin, y = region.yMin, w = region.width(), h = region.height();
        // glm::mat3x3 transform = {
        //         2.0f / w        , 0.0f          , -1.0f - 2.0f * x / w,
        //         0.0f            , 2.0f / h      , -1.0f - 2.0f * y / h,
        //         0.0f            , 0.0f          , 1.0f
        // };

        glm::mat3x3 transform = {
            1,0,0,
            0,1,0,
            0,0,1
        };

        program.setUniform("transformMatrix", transform);
        program.use();
        vertexBuffer.render(GL_TRIANGLES);

    } else {

        // program.remove();

        glBegin(GL_QUADS);
        {
            auto domain = dynamic_cast<const R2toR::DiscreteFunction &>(*func).getDomain();

            auto hPixelSizeInTexCoord = 1. / texture->getWidth();
            auto vPixelSizeInTexCoord = 1. / texture->getHeight();

            auto hTexturePixelSizeInSpaceCoord = hPixelSizeInTexCoord * domain.getLx();
            auto vTexturePixelSizeInSpaceCoord = vPixelSizeInTexCoord * domain.getLy();

            auto si = 0.0; // - hPixelSizeInTexCoord;
            auto sf = 1.0; // + hPixelSizeInTexCoord;
            auto ti = 0.0; // - vPixelSizeInTexCoord;
            auto tf = 1.0; // + vPixelSizeInTexCoord;


            glColor4d(1, 1, 1, 1);

            drawFieldVerts(0.0);

            if (xPeriodic) {
                fix fieldWidth = domain.xMax - domain.xMin;
                for (int i = 1; i <= 2; i++) {
                    glColor4d(1, 1, 1, 0.75 / i);
                    drawFieldVerts(i * fieldWidth);
                    drawFieldVerts(-i * fieldWidth);
                }
            }
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void R2toR::Graphics::FlatFieldDisplay::invalidateTextureData() { validTextureData = false; }

Styles::Color R2toR::Graphics::FlatFieldDisplay::computeColor(Real val) const {
    return cMap.mapValue(logScale ? logAbs(val, cMap_epsArg) : val, cMap_min, cMap_max);
}

void R2toR::Graphics::FlatFieldDisplay::repopulateTextureBuffer() {
    if(func == nullptr || validTextureData) return;

    assert(func->isDiscrete());

    auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

    auto xRes = discreteFunc.getN();
    auto yRes = discreteFunc.getM();

    discreteFunc.getSpace().syncHost();

    for (int i = 0; i < xRes; ++i) {
        for (int j = 0; j < yRes; ++j) {
            auto val = discreteFunc.At(i, j);
            auto color = computeColor(val);

            texture->setColor(i, j, color);
        }
    }

    texture->upload();

    validTextureData = true;
}


bool R2toR::Graphics::FlatFieldDisplay::notifyMouseWheel(int wheel, int direction, int x, int y) {
    constexpr const Real factor = 1.1;
    const Real d = pow(factor, -direction);

    static auto targetRegion = getRegion();

    auto &region = getRegion();
    if(!Core::Graphics::Animator::Contains(region.xMin)
    && !Core::Graphics::Animator::Contains(region.xMax)
    && !Core::Graphics::Animator::Contains(region.yMin)
    && !Core::Graphics::Animator::Contains(region.yMax)) {
        targetRegion = region;
    }

    {
        const Real x0 = targetRegion.xCenter();
        const Real hw = .5 * targetRegion.width() * d;

        targetRegion.xMin = x0-hw;
        targetRegion.xMax = x0+hw;

        set_xMin(targetRegion.xMin);
        set_xMax(targetRegion.xMax);


        const Real y0 = targetRegion.yCenter();
        const Real hh = .5 * targetRegion.height() * d;

        targetRegion.yMin = y0-hh;
        targetRegion.yMax = y0+hh;

        set_yMin(targetRegion.yMin);
        set_yMax(targetRegion.yMax);
    }

    return true;
}

void R2toR::Graphics::FlatFieldDisplay::notifyReshape(int newWinW, int newWinH) {
    Graph2D::notifyReshape(newWinW, newWinH);

    computeGraphRanges();
}

void R2toR::Graphics::FlatFieldDisplay::computeGraphRanges() {
    if(func == nullptr) return;

    auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

    fix vp = getViewport();
    fix domain = discreteFunc.getDomain();

    fix dom_xMin = domain.xMin;
    fix dom_xMax = domain.xMax;
    fix dom_yMin = domain.yMin;
    fix dom_yMax = domain.yMax;

    fix Δx = dom_xMax-dom_xMin;
    fix Δy = dom_yMax-dom_yMin;

    fix windowRatio = (double) vp.height() / vp.width();
    fix fieldRatio = Δy/Δx;
    fix scaleFactor = windowRatio/fieldRatio;

    fix Δx_scaled = Δx/scaleFactor;
    fix xMid = .5*(dom_xMax+dom_xMin);

    let _xMin = xMid - .5*Δx_scaled;
    let _xMax = xMid + .5*Δx_scaled;
    let _yMin = dom_yMin;
    let _yMax = dom_yMin + Δy;

    set_xMin(_xMin);
    set_xMax(_xMax);
    set_yMin(_yMin);
    set_yMax(_yMax);
}

auto R2toR::Graphics::FlatFieldDisplay::getFunction() const -> R2toR::Function::ConstPtr { return func; }

void R2toR::Graphics::FlatFieldDisplay::setColorMap(Styles::ColorMap colorMap) { cMap = colorMap; }

void R2toR::Graphics::FlatFieldDisplay::set_xPeriodicOn() {
    xPeriodic = true;
    texture->set_sPeriodicOn();
}

Str R2toR::Graphics::FlatFieldDisplay::getXHairLabel(const Point2D &coords) {
    auto label = Graph2D::getXHairLabel(coords);

    fix x = Real2D{coords.x, coords.y};
    if(func->domainContainsPoint(x))
        label = "f" + label + " = " + ToStr((*func)(x));


    return label;
}

void R2toR::Graphics::FlatFieldDisplay::drawGUI() {
    Graph2D::drawGUI();

    if (ImGui::Begin("Stats")) {
        if (ImGui::CollapsingHeader((title + " history output").c_str())) {

            if (func->isDiscrete()) {
                auto &dFunc = *dynamic_cast<const DiscreteFunction *>(func.get());
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

                bool antiAlias = texture->getAntiAlias();
                if (ImGui::Checkbox("Anti-alias display", &antiAlias)) {
                    texture->setAntiAlias(antiAlias);
                }

                float fullWidth = ImGui::GetContentRegionAvail().x;
                float relativeWidth = fullWidth * 0.3f;
                ImGui::Text("ϕₘᵢₙ");
                ImGui::SameLine();
                ImGui::PushItemWidth(relativeWidth);
                if (ImGui::SliderFloat("##min", &min, -2, -.005f)) {
                    cMap_min = min;
                    if (symmetricMaxMin) cMap_max = -min;
                    invalidateTextureData();
                }
                ImGui::SameLine();
                if (ImGui::SliderFloat("ϕₘₐₓ", &max, .005f, 2)) {
                    cMap_max = max;
                    if (symmetricMaxMin) cMap_min = -max;
                    invalidateTextureData();
                }
                ImGui::PopItemWidth();

                if (ImGui::DragFloat("ε", &eps, eps * 1e-2, 1e-5, 1e3, "%.6f")) {
                    cMap_epsArg = eps;
                    if (logScale) invalidateTextureData();
                }

                if (ImGui::Checkbox("Log scale", &logScale)) {
                    invalidateTextureData();
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
                    if (item_last_idx != item_current_idx) {
                        cMap = Styles::ColorMaps[selectedItem];
                        invalidateTextureData();
                    }
                    item_last_idx = item_current_idx;
                    ImGui::EndCombo();
                }

                ImGui::Text("ColorMap operations:");
                if (ImGui::Button("Permute")) {
                    cMap = cMap.permute();
                    invalidateTextureData();
                }
                ImGui::SameLine();
                if (ImGui::Button("Odd permute")) {
                    cMap = cMap.bgr();
                    invalidateTextureData();
                }
                ImGui::SameLine();
                if (ImGui::Button("Inverse")) {
                    cMap = cMap.inverse();
                    invalidateTextureData();
                }
                ImGui::SameLine();
                if (ImGui::Button("Reverse")) {
                    cMap = cMap.reverse();
                    invalidateTextureData();
                }
            }
        }
    }
    ImGui::End();

}
