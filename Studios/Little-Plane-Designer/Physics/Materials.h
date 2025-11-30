//
// Created by joao on 11/20/25.
//

#ifndef STUDIOSLAB_MATERIALS_H
#define STUDIOSLAB_MATERIALS_H

constexpr auto LightPlaneDensity = 110.0;
constexpr auto HeavyPlaneDensity = 230.0;

constexpr auto LightRockDensity = 1600.0;
constexpr auto HeavyRockDensity = 3500.0;

constexpr auto ExpandedPolystyreneDensity = 11.0;
constexpr auto ExpandedPolystyreneFriction = 0.01;
constexpr auto ExpandedPolystyreneRestitution = 0.0;

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