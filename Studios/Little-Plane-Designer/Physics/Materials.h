//
// Created by joao on 11/20/25.
//

#ifndef STUDIOSLAB_MATERIALS_H
#define STUDIOSLAB_MATERIALS_H

constexpr auto LightMaterialDensity = 100.0f;
constexpr auto HeavyMaterialDensity = 230.0f;
constexpr auto LightRockDensity = 1600.0f;
constexpr auto HeavyRockDensity = 3500.0f;

constexpr float ExpandedPolystyreneDensity = 11.0f;
constexpr float ExpandedPolystyreneFriction = 0.01f;
constexpr float ExpandedPolystyreneRestitution = 0.0f;

struct FMaterial {
    const Real64 Density;
    const Real64 Friction;
    const Real64 Restitution;
};

constexpr FMaterial ExpandedPolystyrene = {
    .Density = 11,
    .Friction = .01,
    .Restitution = .0
};

#endif //STUDIOSLAB_MATERIALS_H