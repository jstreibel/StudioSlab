//
// Created by joao on 11/21/25.
//

#include "FLittlePlaneBlueprint.h"
#include "../Physics/Materials.h"
#include "../Physics/Foils/NACA2412.h"
#include "Utils/Angles.h"

#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/SFML/Graph.h"

void FLittlePlaneBlueprint::Tick(Seconds ElapsedTime) {

}

void FLittlePlaneBlueprint::TogglePause() {

}

void FLittlePlaneBlueprint::Draw(const Graphics::FDrawInput& Input) {
    Input.Window.Clear(Graphics::DarkGrey);

    fix WinHeight = Input.Window.GetHeight();
    fix WinWidth = Input.Window.GetWidth();
    fix AspectRatio = static_cast<double>(WinWidth)/WinHeight;

    namespace Draw = Graphics::OpenGL::Legacy;
    constexpr auto ArchD_height = 0.457; //m
    constexpr auto ArchD_width = 0.610; // m
    constexpr auto PaperHeight = ArchD_height/2;
    constexpr auto PaperWidth = PaperHeight * 16 / 9.;

    const auto ViewportHeight_2 = PaperHeight * 1.2 / 2.0;
    const auto ViewportWidth_2 = ViewportHeight_2 * AspectRatio;

    Draw::SetupOrtho({-ViewportWidth_2, ViewportWidth_2, -ViewportHeight_2, ViewportHeight_2});

    const Graphics::FRectangleShape ArchD = {{-ArchD_width/2, +ArchD_height/2}, {+ArchD_width/2, -ArchD_height/2}};
    const Graphics::FRectangleShape UnitSquare = {{-0.1, 0.1}, {+0.1, -0.1}};
    const Graphics::FRectangleShape Paper = {{-PaperWidth/2, +PaperHeight/2}, {+PaperWidth/2, -PaperHeight/2}};

    Draw::SetColor(Graphics::Blueprint);
    Draw::DrawRectangle(Paper);

    const auto OuterX = floorf(100*PaperWidth/2)/100;
    const auto OuterY = floorf(100*PaperHeight/2)/100;
    const auto InnerX = OuterX - 0.01;
    const auto InnerY = OuterY - 0.005;

    constexpr auto TickSize = 0.00125;

    const auto BorderColor = Graphics::WhiteBlueprintStroke;
    const auto LineColor = Graphics::GreyBlue*0.75;

    auto DrawRect = [&](double hX, double hY, double lw) {
        Draw::DrawLine(
        {-hX, hY},
        {hX,  hY},
        BorderColor, lw);

        Draw::DrawLine(
            {-hX, -hY},
            {hX,  -hY},
            BorderColor, lw);

        Draw::DrawLine(
            {-hX, -hY},
            {-hX, +hY},
            BorderColor, lw);

        Draw::DrawLine(
            {hX, -hY},
            {hX, +hY},
            BorderColor, lw);
    };

    DrawRect(OuterX, OuterY, 2.5);
    DrawRect(InnerX, InnerY, 2.5);

    Draw::DrawLine({-InnerX, 0}, {InnerX, 0}, LineColor, 2.5);
    Draw::DrawLine({0, InnerY}, {0, -InnerY}, LineColor, 2.5);

    for (float x = .01; x < InnerX; x+=0.01) {
        Draw::DrawLine({x, InnerY}, {x, -InnerY}, LineColor, 1);
        Draw::DrawLine({-x, InnerY}, {-x, -InnerY}, LineColor, 1);

        Draw::DrawLine({+x, -InnerY}, {+x, -InnerY+TickSize}, BorderColor, 2.5);
        Draw::DrawLine({-x, -InnerY}, {-x, -InnerY+TickSize}, BorderColor, 2.5);
    }

    for (float x = .005; x < InnerX; x+=0.01) {
        Draw::DrawLine({x, InnerY}, {x, -InnerY}, LineColor, .25);
        Draw::DrawLine({-x, InnerY}, {-x, -InnerY}, LineColor, .25);
    }

    for (float y = .01; y < InnerY; y+=.01) {
        Draw::DrawLine({-InnerX, y}, {InnerX, y}, LineColor, 1);
        Draw::DrawLine({-InnerX, -y}, {InnerX, -y}, LineColor, 1);

        Draw::DrawLine({-InnerX, +y}, {-InnerX+TickSize, +y}, BorderColor, 2.5);
        Draw::DrawLine({-InnerX, -y}, {-InnerX+TickSize, -y}, BorderColor, 2.5);
    }

    for (float y = .005; y < InnerY; y+=.01) {
        Draw::DrawLine({-InnerX, y}, {InnerX, y}, LineColor, .25);
        Draw::DrawLine({-InnerX, -y}, {InnerX, -y}, LineColor, .25);
    }
}

void FLittlePlaneBlueprint::Startup(const Graphics::FPlatformWindow&) {

}

void FLittlePlaneBlueprint::HandleInputState(FInputState) {

}

TPointer<FPlaneFactory> SetupPlane();

TPointer<FLittlePlane> FLittlePlaneBlueprint::BuildPlane(const b2WorldId World) {
    return GetPlaneFactory()->BuildPlane(World);
}

auto FLittlePlaneBlueprint::GetPlaneFactory() -> TPointer<FPlaneFactory> {
    if (PlaneFactory == nullptr) PlaneFactory = SetupPlane();

    return PlaneFactory;
}

TPointer<FPlaneFactory> SetupPlane() {
    auto Factory = New<FPlaneFactory>();

    Factory.get()
           ->SetPosition({161.5f, 41.f})
           .SetRotation(DegToRad(0.0f))
           .AddBodyPart({
               .Density = LightMaterialDensity,
               .Width = 4.0f,
               .Height = 0.5,
           })
           .AddBodyPart({
               .Density = HeavyMaterialDensity,
               .Width = 1,
               .Height = 0.25,
               .xOffset = -1.2,
           })
           .AddBodyPart({
               .Density = LightMaterialDensity,
               .Width = 0.4,
               .Height = 0.4,
               .xOffset = -1.0,
               .yOffset = -0.5,
           })
           .AddWing(FWingDescriptor{
               .Density = LightMaterialDensity,
               .Airfoil = New<Foil::ViternaAirfoil2412>(),
               .Params = Foil::FAirfoilParams{
                   .Name = "Wing",
                   .ChordLength = 1.15f,
                   .Span = 6.0f,
               },
               .RelativeLocation = {-.6f, -0.1f},
               .BaseAngle = static_cast<float>(DegToRad(0.0)),
               .MaxAngle  = static_cast<float>(DegToRad(15)),
               .MinAngle  = static_cast<float>(DegToRad(-15)),
           })
           .AddWing(FWingDescriptor{
               .Density = LightMaterialDensity,
               .Airfoil = New<Foil::ViternaAirfoil2412>(),
               .Params = Foil::FAirfoilParams{
                   .Name = "Winglet",
                   .ChordLength = 0.6f,
                   .Span = 2.f,
               },
               .RelativeLocation = {+1.4, 0.0f},
               .BaseAngle = static_cast<float>(DegToRad(0.0)),
               .MaxAngle  = static_cast<float>(DegToRad(15)),
               .MinAngle  = static_cast<float>(DegToRad(-15)),
               .OscFreq = 10.0f,
               .DampRatio = 1.0f,
           });

    return Factory;
}