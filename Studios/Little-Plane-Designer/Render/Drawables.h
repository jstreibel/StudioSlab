//
// Created by joao on 11/14/25.
//

#ifndef STUDIOSLAB_DRAWABLES_H
#define STUDIOSLAB_DRAWABLES_H

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Draw.h"

struct FDecal : Slab::Graphics::IDrawable2D {
    FDecal() = delete;
    explicit FDecal(
        const Slab::Str& LittlePlaneDesigner_FileName,
        Slab::Graphics::FPoint2D Position = {0,0},
        Slab::Graphics::FPoint2D Size = {1,1}, Slab::DevFloat ZIndex = 0.0f);

    void Draw(const Slab::Graphics::FDraw2DParams&) override;

    Slab::Graphics::FPoint2D Position={0,0};
    Slab::DevFloat ZIndex = 0.0f;
    Slab::Graphics::FPoint2D Size={1,1};

private:
    const Slab::TPointer<Slab::Graphics::OpenGL::FTexture> Texture;
};

struct FGuy final : FDecal {
    explicit FGuy(
        const Slab::Graphics::FPoint2D Position = {0,0},
        const Slab::DevFloat ZIndex=0.0f)
    : FDecal("Guy01.png", Position, {2.f,2.f}, ZIndex) { }
};

struct FCat final : FDecal
{
    explicit FCat(
    const Slab::Graphics::FPoint2D Position = {0,0},
    const Slab::DevFloat ZIndex=0.0f)
    : FDecal("Cat02.png", Position, {0.32f,0.32f}, ZIndex) { }
};

struct FGarage final : FDecal
{
    explicit FGarage(
    const Slab::Graphics::FPoint2D Position = {0,0},
    const Slab::DevFloat ZIndex=0.0f)
    : FDecal("Garage01.png", Position, {3.0f,3.0f}, ZIndex) {}
};

struct FTree01 final : FDecal
{
    explicit FTree01(
    const Slab::Graphics::FPoint2D Position = {0,0},
    const Slab::DevFloat ZIndex=0.0f)
    : FDecal("Tree01.png", Position, {4.0f,4.0f}, ZIndex) { };
};

struct FRuler final : Slab::Graphics::IDrawable2D {
    Slab::Graphics::FPoint2D Loc = {0.0f, 0.0f};
    float Unit = 1.0f;
    float RulerWidth = .05;
    floatt ZIndex = 0.0f;

    explicit FRuler(
        const Slab::Graphics::FPoint2D &Loc={.0f,.0f},
        const float &Unit=1.0f,
        Slab::DevFloat ZIndex=0.0f);

    void Draw(const Slab::Graphics::FDraw2DParams&) override;
};

struct FSky final : Slab::Graphics::IDrawable2D {
    void Draw(const Slab::Graphics::FDraw2DParams&) override;

    Slab::Graphics::FColor HorizonColor = {0.65f, 0.80f, 0.95f, 1.0f};
    Slab::Graphics::FColor SkyColor = {0.10f, 0.30f, 0.55f, 1.0f};
};

#endif //STUDIOSLAB_DRAWABLES_H