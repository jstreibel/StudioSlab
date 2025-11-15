//
// Created by joao on 11/14/25.
//

#ifndef STUDIOSLAB_DRAWABLES_H
#define STUDIOSLAB_DRAWABLES_H

#include "../../../Lib/Graphics/Interfaces/IDrawable.h"
#include "Graphics/Types2D.h"

struct FGuy final : Slab::Graphics::IDrawable {
    Slab::Graphics::Point2D Position={0,0};

    explicit FGuy(const Slab::Graphics::Point2D Position = {0,0}) : Position(Position) { };

    void Draw(const Slab::Graphics::IDrawProviders&) override;
};

struct FCat final : Slab::Graphics::IDrawable
{
    Slab::Graphics::Point2D Position;

    explicit FCat(const Slab::Graphics::Point2D Position = {0,0}) : Position(Position) { };

    void Draw(const Slab::Graphics::IDrawProviders&) override;
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