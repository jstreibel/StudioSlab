//
// Created by joao on 4/8/23.
//

#include "FlatFieldDisplay.h"


#include "Utils/Resources.h"

#include "Graphics/Styles/ColorMap.h"
#include "Core/Tools/Log.h"
#include "Core/Tools/Animator.h"

#include "Maps/R2toR/Model/R2toRDiscreteFunction.h"

#include "imgui.h"
#include "Graphics/OpenGL/Texture2D_Color.h"
#include "Graphics/Graph/StylesManager.h"

#include <utility>

#define ODD true

struct FlatFieldVertex {
    float x, y;     // position
    float s, t;     // texture
};

struct TestVertex {
    float x, y;
    float s, t;
};

R2toR::Graphics::FlatFieldDisplay::FlatFieldDisplay(Str title, Real phiMin, Real phiMax)
: ::Graphics::Graph2D(-1, 1, -1, 1, std::move(title))
, colorBar({50,150, 50, 750})
, cMap_min(phiMin)
, cMap_max(phiMax)
, symmetricMaxMin(Common::areEqual(phiMax,-phiMin))
, vertexBuffer("vertex:2f,tex_coord:2f")
, program(Resources::ShadersFolder+"FlatField.vert", Resources::ShadersFolder+"FlatField.frag")
{
    computeColormapTexture();

    program.setUniform("colormap", cMap_texture->getTextureUnit());
    program.setUniform("phiMin", (GLfloat)cMap_min);
    program.setUniform("phiMax", (GLfloat)cMap_max);
    program.setUniform("eps", (GLfloat)cMap_epsArg);

    addArtist(DummyPtr(colorBar));
};

void R2toR::Graphics::FlatFieldDisplay::setFunction(R2toR::Function::ConstPtr function, const Unit& unit) {
    if(func == function) return;

    bool firstTime = func == nullptr;
    func = std::move(function);
    funcUnit = unit;
    if(firstTime) computeGraphRanges();

    if(!func->isDiscrete()) NOT_IMPLEMENTED

    auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

    auto xRes = discreteFunc.getN();
    auto yRes = discreteFunc.getM();

    if(firstTime
    || textureData->getWidth() != xRes
    || textureData->getHeight() != yRes)
    {
        delete textureData;
        textureData = new ::Graphics::OpenGL::Texture2D_Real((int) xRes, (int) yRes);
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

        Log::Debug() << "Generated vertices for '" << title << "' graph VertexBuffer. ";
    }

    invalidateTextureData();
    repopulateTextureBuffer();
}

void R2toR::Graphics::FlatFieldDisplay::draw() {
    if(true) {
        Graph2D::draw();
    }
    else {
        // Don't call Graphics::Graph2D::draw();
        Window::setClearColor(Math::StylesManager::GetCurrent()->graphBackground);
        Window::draw();
        setupOrtho();
        labelingHelper.setTotalItems(countDisplayItems());
        artistsDraw();
        drawPointSets();
        drawCurves();
        drawGUI();
    }

    drawFlatField();
}

void R2toR::Graphics::FlatFieldDisplay::drawFlatField() {
    if (func == nullptr) return;

    if (!validTextureData)
        repopulateTextureBuffer();

    auto region = this->getRegion();
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

}

void R2toR::Graphics::FlatFieldDisplay::computeColormapTexture() {
    delete cMap_texture;

    fix nColors = 1024;
    cMap_texture = new ::Graphics::OpenGL::Texture1D_Color(nColors, GL_TEXTURE1);
    cMap_texture->setWrap(::Graphics::OpenGL::ClampToEdge);

    for(auto i=0; i<nColors; ++i){
        fix s = (Real)(i-1)/(Real)(nColors-2);
        fix color = cMap.mapValue(s, 0, 1);
        cMap_texture->setColor(i, color);
    }

    cMap_texture->upload();

    colorBar.setTexture(DummyPtr(*cMap_texture));
}

void R2toR::Graphics::FlatFieldDisplay::invalidateTextureData() { validTextureData = false; }

void R2toR::Graphics::FlatFieldDisplay::repopulateTextureBuffer() {
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

bool R2toR::Graphics::FlatFieldDisplay::notifyMouseWheel(double dx, double dy) {

    constexpr const Real factor = 1.1;
    const Real d = pow(factor, -dy);

    static auto targetRegion = getRegion();

    auto &region = getRegion();
    if(!Core::Animator::Contains(region.xMin)
    && !Core::Animator::Contains(region.xMax)
    && !Core::Animator::Contains(region.yMin)
    && !Core::Animator::Contains(region.yMax)) {
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

    colorBar.setLocation({newWinW-100, newWinW-50, newWinH-700, newWinH-200});

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

void R2toR::Graphics::FlatFieldDisplay::setColorMap(const Styles::ColorMap& colorMap) {
    cMap = colorMap;
    computeColormapTexture();
}

void R2toR::Graphics::FlatFieldDisplay::set_xPeriodicOn() {
    textureData->set_sPeriodicOn();
}

Str R2toR::Graphics::FlatFieldDisplay::getXHairLabel(const ::Graphics::Point2D &coords) const {
    auto label = Graph2D::getXHairLabel(coords);

    if(func == nullptr) return label;

    auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

    auto xRes = discreteFunc.getN();
    auto yRes = discreteFunc.getM();

    auto domain = discreteFunc.getDomain();

    auto hPixelSizeInTexCoord = 1. / xRes;
    auto vPixelSizeInTexCoord = 1. / yRes;

    auto hTexturePixelSizeInSpaceCoord = hPixelSizeInTexCoord * domain.getLx();
    auto vTexturePixelSizeInSpaceCoord = vPixelSizeInTexCoord * domain.getLy();

    fix r = Real2D{coords.x, coords.y};
    fix rMin = Real2D{coords.x+.5*hTexturePixelSizeInSpaceCoord, coords.y+.5*vTexturePixelSizeInSpaceCoord};
    fix rMax = Real2D{coords.x-.5*hTexturePixelSizeInSpaceCoord, coords.y-.5*vTexturePixelSizeInSpaceCoord};
    if(func->domainContainsPoint(r)) {
        fix val = (*func)(r);
        label = "f" + label + " = " + funcUnit(val, 5);
    } else if(func->domainContainsPoint(rMin)) {
        fix val = (*func)(rMin);
        label = "f" + label + " = " + funcUnit(val, 5);
    } else if(func->domainContainsPoint(rMax)) {
        fix val = (*func)(rMax);
        label = "f" + label + " = " + funcUnit(val, 5);
    }

    return label;
}

void R2toR::Graphics::FlatFieldDisplay::drawGUI() {
    Graph2D::drawGUI();

    if(func == nullptr) return;

    if (ImGui::Begin("Stats")) {
        if (ImGui::CollapsingHeader((title + " history output").c_str())) {

            const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
            ImGui::BeginChild((title + "history output##1").c_str(), {0,16*TEXT_BASE_HEIGHT}, true/*, ImGuiWindowFlags_AlwaysAutoResize*/);

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

                if (ImGui::DragFloat("ε", &eps, (float)eps * 1e-2, 1e-5, 1e3, "%.6f")) {
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
                    if (item_last_idx != item_current_idx) {
                        cMap = Styles::ColorMaps[selectedItem];
                        computeColormapTexture();
                    }
                    item_last_idx = item_current_idx;
                    ImGui::EndCombo();
                }

                ImGui::Text("ColorMap operations:");
                if (ImGui::Button("Permute")) {
                    cMap = cMap.permute();
                    computeColormapTexture();
                }
                ImGui::SameLine();
                if (ImGui::Button("RGB -> BGR")) {
                    cMap = cMap.bgr();
                    computeColormapTexture();
                }
                ImGui::SameLine();
                if (ImGui::Button("Inverse")) {
                    cMap = cMap.inverse();
                    computeColormapTexture();
                }
                ImGui::SameLine();
                if (ImGui::Button("Reverse")) {
                    cMap = cMap.reverse();
                    computeColormapTexture();
                }
            }

            ImGui::EndChild();
        }
    }
    ImGui::End();

}


