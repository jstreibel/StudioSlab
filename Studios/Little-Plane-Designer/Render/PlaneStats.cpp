//
// Created by joao on 11/7/25.
//

#include "PlaneStats.h"
#include <cstdarg>
#include <cstdio>
#include <cmath>

// printf-style formatter that returns Slab::Str
static Str Print(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    va_list copy;
    va_copy(copy, args);
    int n = std::vsnprintf(nullptr, 0, fmt, copy);
    va_end(copy);
    Str out;
    if (n > 0) {
        out.resize(static_cast<size_t>(n) + 1);
        std::vsnprintf(&out[0], static_cast<size_t>(n) + 1, fmt, args);
        out.resize(static_cast<size_t>(n));
    }
    va_end(args);
    return out;
}

void FPlaneStats::Draw(const Graphics::FDraw2DParams& Params) {
    fix W = Params.ScreenWidth;
    fix H = Params.ScreenHeight;
    Writer->Reshape(W, H);

    Graphics::OpenGL::SetViewport(Graphics::RectI{
        .xMin = 0, .xMax = W, .yMin = 0, .yMax = H
    });

    const float fontH = Writer->GetFontHeightInPixels();
    int n = 0;

    constexpr double MpsToKmph = 3.6;                 // 3.6
    constexpr double MpsToMph  = 3600.0 / 1609.344;   // ≈ 2.2369362920544

    const float speed = Plane->GetVelocity().norm();
    const float speedKmh = speed * static_cast<float>(MpsToKmph);
    const float speedMph = speed * static_cast<float>(MpsToMph);
    const float planeHeight = Plane->GetPosition().y;

    // Weight vs aero vertical force
    const float m  = Plane->GetTotalMass();
    // const float g  = b2Length(b2World_GetGravity(World));

    float sumFy = 0.0f;
    float sumWingMass = 0.0f;
    struct WingInfo {
        const char *name;
        float mass;
        float Cl;
        float Cd;
        float Fy;
        float AoAdeg;
    };
    Vector<WingInfo> infos;
    const int nWings = Plane->GetWingCount();
    infos.reserve(nWings);
    for (int i = 0; i < nWings; ++i) {
        const auto& wing = Plane->GetWing(i);
        const float wingMass = b2Body_GetMass(wing.BodyId);
        sumWingMass += wingMass;
        const auto data = FLittlePlane::ComputeForces(wing);
        const float Fy = data.GetTotalForce().y;
        sumFy += Fy;
        const float AoAdeg = data.AoA * static_cast<float>(180.0 / M_PI);
        infos.push_back(WingInfo{wing.Params.Name.c_str(), wingMass, data.Cl, data.Cd, Fy, AoAdeg});
    }

    const auto x = W - 1200.0f;
    const auto baseY = H - 100.0f;
    const auto dy = 1.2f * fontH;

    Writer->Write(Print("Speed %.2f m/s  |  %.2f km/h  |  %.2f mph", speed, speedKmh, speedMph), {x, baseY - static_cast<float>(n++)*dy});
    Writer->Write(Print("Height %.2f m", planeHeight),                                                 {x, baseY - static_cast<float>(n++)*dy});

    // Mass breakdown
    Writer->Write(Print("mₜₒₜₐₗ = %.2f kg", m),                                                         {x, baseY - static_cast<float>(n++)*dy});

    // Per-wing aero breakdown
    for (int i = 0; i < nWings; ++i) {
        Writer->Write(Print("%s: AoA=%.1f°", infos[i].name, infos[i].Cl, infos[i].Cd, infos[i].AoAdeg, infos[i].Fy),
                      {x, baseY - static_cast<float>(n++)*dy});
    }
}
