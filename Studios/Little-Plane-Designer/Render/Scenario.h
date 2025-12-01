//
// Created by joao on 12/18/25.
//

#ifndef STUDIOSLAB_FSCENARIO_H
#define STUDIOSLAB_FSCENARIO_H

#include "Graphics/Interfaces/IDrawable.h"
#include "Graphics/Styles/Colors.h"

struct FScenario final : Slab::Graphics::IDrawable2D {
    void Draw(const Slab::Graphics::FDraw2DParams&) override;

    explicit FScenario();

private:
    struct FMountainLayer {
        float Parallax = 1.0f;
        float BaseHeight = 0.0f;
        float Amplitude = 0.0f;
        float Frequency = 0.0f;
        float Phase = 0.0f;
        float Depth = -0.5f;
        Slab::Graphics::FColor Color;
    };

    static float ApplyParallax(float x, float parallax, float viewCenter);
    static float Ridge(const float x, const FMountainLayer& layer);

    Slab::TList<FMountainLayer> Layers;
};

#endif //STUDIOSLAB_FSCENARIO_H
