//
// Created by joao on 11/14/25.
//

#ifndef STUDIOSLAB_DRAWABLES_H
#define STUDIOSLAB_DRAWABLES_H

#include "../../../Lib/Graphics/Interfaces/IDrawable.h"
#include "Graphics/Types2D.h"
#include "Graphics/OpenGL/Texture.h"

struct FDecal : Slab::Graphics::IDrawable {
    FDecal() = delete;
    explicit FDecal(
        const Slab::Str& LittlePlaneDesigner_FileName,
        Slab::Graphics::Point2D Position = {0,0},
        Slab::Graphics::Point2D Size = {1,1});

    void Draw(const Slab::Graphics::IDrawProviders&) override;

    Slab::Graphics::Point2D Position={0,0};
    Slab::Graphics::Point2D Size={1,1};

private:
    const Slab::TPointer<Slab::Graphics::OpenGL::FTexture> Texture;
};

struct FGuy final : FDecal {
    explicit FGuy(const Slab::Graphics::Point2D Position = {0,0})
    : FDecal("Guy.png", Position, {2.f,2.f}) { };
};

struct FCat final : FDecal
{
    explicit FCat(const Slab::Graphics::Point2D Position = {0,0})
    : FDecal("Cat.png", Position, {0.32f,0.32f}) { };
};

struct FTree01 final : FDecal
{
    explicit FTree01(const Slab::Graphics::Point2D Position = {0,0})
    : FDecal("Tree01.png", Position, {4.0f,4.0f}) { };
};

struct FRuler final : Slab::Graphics::IDrawable {
    Slab::Graphics::Point2D Loc = {0.0f, 0.0f};
    float Unit = 1.0f;
    float RulerWidth = .05;

    explicit FRuler(const Slab::Graphics::Point2D &Loc={.0f,.0f}, const float &Unit=1.0f);

    void Draw(const Slab::Graphics::IDrawProviders&) override;
};

struct FSky final : Slab::Graphics::IDrawable {
    void Draw(const Slab::Graphics::IDrawProviders&) override;
};

#endif //STUDIOSLAB_DRAWABLES_H