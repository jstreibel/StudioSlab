//
// Created by joao on 12/18/25.
//

#include "Scenario.h"

#include <algorithm>
#include <cmath>

#include "Graphics/OpenGL/LegacyGL/LegacyMode.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"

using Slab::Graphics::FColor;

constexpr float k_BaseFrequency = 0.03f;



float FScenario::ApplyParallax(const float x, const float parallax, const float viewCenter) {
    return parallax * x + (1.0f - parallax) * viewCenter;
}

float FScenario::Ridge(const float x, const FMountainLayer& layer) {
    const float base = std::sin(layer.Frequency * x + layer.Phase);
    fix f = layer.Frequency;
    const float slow = 0.55f * std::sin(0.55f*f*x + 0.35f + 0.65f * layer.Phase);
    const float fast = 0.35f * std::sin(1.90f*f*x + 1.10f * layer.Phase);

    const float crest = std::abs(base + slow + fast);
    return layer.BaseHeight + layer.Amplitude * crest;
}

void FScenario::Draw(const Slab::Graphics::FDraw2DParams& Params) {
    namespace LegacyGL = Slab::Graphics::OpenGL::Legacy;

    LegacyGL::SetupLegacyGL();
    LegacyGL::PushLegacyMode();
    LegacyGL::PushScene();
    LegacyGL::ResetModelView();
    LegacyGL::SetupOrtho(Params.Region);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const float viewWidth = Params.Region.GetWidth();
    const float sampleStep = std::max(0.5f, viewWidth / 100.0f);
    const float baseY = Params.Region.yMin - 15.0f;
    const float xCenter = Params.Region.xCenter();
    const float yCenter = Params.Region.yCenter();

    const float parallaxMargin = 0.5f * viewWidth;

    for (const auto& layer : Layers) {
        glColor4f(layer.Color.r, layer.Color.g, layer.Color.b, layer.Color.a);

        const float parallaxScale = std::max(layer.Parallax, 0.05f);
        const float xStart = Params.Region.xMin - parallaxMargin / parallaxScale;
        const float xEnd = Params.Region.xMax + parallaxMargin / parallaxScale;

        glBegin(GL_TRIANGLE_STRIP);
        for (float s = xStart; s <= xEnd + sampleStep; s += sampleStep) {
            const float x = ApplyParallax(s, layer.Parallax, xCenter);
            const float y = ApplyParallax(Ridge(s, layer), layer.Parallax, yCenter);
            glVertex3f(x, baseY, layer.Depth);
            glVertex3f(x, y, layer.Depth);
        }
        glEnd();
    }

    LegacyGL::PopScene();
}

FScenario::FScenario() {
    Layers = {{
        {
            .Parallax = 0.07f,
            // .BaseHeight = 14.5f,
            .BaseHeight = 5.5f,
            .Amplitude = 22.5f,
            // .Frequency = 0.085f,
            .Frequency = k_BaseFrequency*0.2,
            .Phase = 0.0f,
            .Depth = -0.95f,
            .Color = FColor::FromHex("#0d2f4c")*0.5
        },
        {
            .Parallax = 0.18f,
            // .BaseHeight = 14.5f,
            .BaseHeight = 2.5f,
            .Amplitude = 5.5f,
            // .Frequency = 0.085f,
            .Frequency = k_BaseFrequency,
            .Phase = 0.0f,
            .Depth = -0.95f,
            .Color = FColor::FromHex("#0d2f4c")
        },
        {
            .Parallax = 0.35f,
            // .BaseHeight = 12.0f,
            .BaseHeight = 1.0f,
            .Amplitude = 1.0f,
            //.Frequency = 0.120f,
            .Frequency = k_BaseFrequency,
            .Phase = 1.3f,
            .Depth = -0.90f,
            .Color = FColor::FromHex("#124166")
        },
        {
            .Parallax = 0.55f,
            // .BaseHeight = 9.0f,
            .BaseHeight = 0.5f,
            .Amplitude = 3.0f,
            //.Frequency = 0.185f,
            .Frequency = k_BaseFrequency,
            .Phase = 2.45f,
            .Depth = -0.85f,
            .Color = FColor::FromHex("#195276")
        },
    }};
}
