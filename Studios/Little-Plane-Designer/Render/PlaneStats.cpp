//
// Created by joao on 11/7/25.
//

#include "PlaneStats.h"
#include <cstdarg>
#include <cstdio>

void FPlaneStats::Draw(const Graphics::FPlatformWindow& Win) {
    fix W = Win.GetWidth();
    fix H = Win.GetHeight();
    Writer->Reshape(W, H);

    fix FontHeight = Writer->GetFontHeightInPixels();
    auto n = 0;

    constexpr double MpsToKmph = 3.600 / 1.000;       // 3.6
    constexpr double MpsToMph = 3600.0 / 1609.344;  // ≈ 2.2369362920544

    fix Speed = b2Length(Plane->GetVelocity());
    fix SpeedKmh = Speed * MpsToKmph;
    fix SpeedMph = Speed * MpsToMph;
    fix PlaneHeight = Plane->GetPosition().y;

    constexpr auto xPos = -700;

    Writer->Write(ToStr("Speed %.2f m/s - %.2f km/h - %.2f mph", Speed, SpeedKmh, SpeedMph), {W+xPos, H-100-static_cast<int>(n++ * 1.2 * FontHeight)});

    Writer->Write(ToStr("Height %.2f m", PlaneHeight), {W-600, H-100-static_cast<int>(n++ * 1.2 * FontHeight)});
}
