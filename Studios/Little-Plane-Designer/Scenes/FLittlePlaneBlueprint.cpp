//
// Created by joao on 11/21/25.
//

#include "FLittlePlaneBlueprint.h"
#include "../Physics/Materials.h"
#include "../Physics/Foils/NACA2412.h"
#include "Utils/Angles.h"

#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"
#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/SFML/Graph.h"

constexpr auto ArchD_height = 0.457; //m
constexpr auto ArchD_width = 0.610; // m
constexpr auto PaperHeight = ArchD_height/2;
constexpr auto PaperWidth = PaperHeight * 16 / 9.;
constexpr auto ViewportHeight_2 = PaperHeight * 1.2 / 2.0;

constexpr auto Width_StrongStroke = 2.5;
constexpr auto Width_RegularStroke = 1.5;
constexpr auto Width_WeakStroke = 0.8;

const auto BackgroundColor = Graphics::WoodLight;
const auto BorderColor = Graphics::WhiteBlueprintStroke;
const auto LineColor = Graphics::GreyBlue*0.75;
const auto StrokeColor = BorderColor;
const auto BlueprintColor = Graphics::BlueprintLight;

Graphics::PlotStyle StrongStrokeStyle{StrokeColor, Graphics::LineLoop, false, Graphics::Nil, Width_StrongStroke};
Graphics::PlotStyle RegularStrokeStyle{StrokeColor, Graphics::LineLoop, false, Graphics::Nil, Width_RegularStroke};
Graphics::PlotStyle WeakStrokeStyle{StrokeColor, Graphics::LineLoop, false, Graphics::Nil, Width_WeakStroke};

namespace Draw = Graphics::OpenGL::Legacy;


void FLittlePlaneBlueprint::Tick(Seconds ElapsedTime) {

}

void FLittlePlaneBlueprint::TogglePause() {

}

void FLittlePlaneBlueprint::Draw(const Graphics::FDrawInput& Input) {
    Input.Window.Clear(BackgroundColor);

    fix WinHeight = Input.Window.GetHeight();
    fix WinWidth = Input.Window.GetWidth();
    fix AspectRatio = static_cast<double>(WinWidth)/WinHeight;

    const auto ViewportWidth_2 = ViewportHeight_2 * AspectRatio;
    Draw::SetupOrtho({-ViewportWidth_2, ViewportWidth_2, -ViewportHeight_2, ViewportHeight_2});

    DrawBlueprint();

    DrawPlane();
}

void FLittlePlaneBlueprint::Startup(const Graphics::FPlatformWindow&) {
}

void FLittlePlaneBlueprint::HandleInputState(FInputState) {

}

TPointer<FPlaneFactory> SetupDefaultPlane();

TPointer<FLittlePlane> FLittlePlaneBlueprint::BuildPlane(const b2WorldId World) {
    return GetPlaneFactory()->BuildPlane(World);
}

auto FLittlePlaneBlueprint::GetPlaneFactory() -> TPointer<FPlaneFactory> {
    if (PlaneFactory == nullptr) PlaneFactory = SetupDefaultPlane();

    return PlaneFactory;
}

void FLittlePlaneBlueprint::DrawBlueprint() {
    const Graphics::FRectangleShape ArchD = {{-ArchD_width/2, +ArchD_height/2}, {+ArchD_width/2, -ArchD_height/2}};
    const Graphics::FRectangleShape UnitSquare = {{-0.1, 0.1}, {+0.1, -0.1}};
    const Graphics::FRectangleShape Paper = {{-PaperWidth/2, +PaperHeight/2}, {+PaperWidth/2, -PaperHeight/2}};

    Draw::SetColor(BlueprintColor);
    Draw::DrawRectangle(Paper);

    const auto OuterX = floorf(100*PaperWidth/2)/100;
    const auto OuterY = floorf(100*PaperHeight/2)/100;
    const auto InnerX = OuterX - 0.01;
    const auto InnerY = OuterY - 0.005;

    constexpr auto TickSize = 0.00125;

    for (float x = .005; x < InnerX; x+=0.01) {
        Draw::DrawLine({x, InnerY}, {x, -InnerY}, LineColor, .25);
        Draw::DrawLine({-x, InnerY}, {-x, -InnerY}, LineColor, .25);
    }

    for (float y = .005; y < InnerY; y+=.01) {
        Draw::DrawLine({-InnerX, y}, {InnerX, y}, LineColor, .25);
        Draw::DrawLine({-InnerX, -y}, {InnerX, -y}, LineColor, .25);
    }

    for (float y = .01; y < InnerY; y+=.01) {
        Draw::DrawLine({-InnerX, y}, {InnerX, y}, LineColor, 1);
        Draw::DrawLine({-InnerX, -y}, {InnerX, -y}, LineColor, 1);

        Draw::DrawLine({-InnerX, +y}, {-InnerX+TickSize, +y}, BorderColor, 2.5);
        Draw::DrawLine({-InnerX, -y}, {-InnerX+TickSize, -y}, BorderColor, 2.5);
    }

    for (float x = .01; x < InnerX; x+=0.01) {
        Draw::DrawLine({x, InnerY}, {x, -InnerY}, LineColor, 1);
        Draw::DrawLine({-x, InnerY}, {-x, -InnerY}, LineColor, 1);

        Draw::DrawLine({+x, -InnerY}, {+x, -InnerY+TickSize}, BorderColor, 2.5);
        Draw::DrawLine({-x, -InnerY}, {-x, -InnerY+TickSize}, BorderColor, 2.5);
    }

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
}

void RenderCOM(Math::Point2D COM) {

    constexpr auto Radius = PaperHeight / 35.;
    auto Points = Math::Geometry::FCircle{COM, Radius}.GetPoints();

    Draw::RenderPointSet(Dummy(Points), WeakStrokeStyle);

    Draw::DrawLine({COM.x - Radius, COM.y}, {COM.x + Radius, COM.y}, StrokeColor, Width_WeakStroke);
    Draw::DrawLine({COM.x, COM.y - Radius}, {COM.x, COM.y + Radius}, StrokeColor, Width_WeakStroke);

}

void FLittlePlaneBlueprint::DrawPlane() {
    fix Scale = 1./static_cast<double>(Proportion);

    Math::Point2D COM = {0,0};
    Real64 Mass = 0;

    for (const auto &Part : GetPlaneFactory()->BodyPartDescriptors) {
        auto Points = FBodyPartGeometry{Part}.GetPoints();

        auto COMResult = Math::Geometry::ComputeCentroid(Points);
        if (COMResult.IsFailure()) continue;

        auto Area = Math::Geometry::ComputeArea(Points);

        fix MyCOM = COMResult.Value();
        fix MyMass = Area * Part.Density;

        COM += MyCOM * MyMass;
        Mass += MyMass;

        Draw::RenderPointSet(Dummy(Points*=Scale), StrongStrokeStyle);
    }

    RenderCOM(COM * Scale / Mass);

    for (const auto &Wing : GetPlaneFactory()->WingDescriptors) {
        const auto& [x, y] = Wing.RelativeLocation;
        auto ProfileVertices = Wing.Airfoil->GetProfileVertices(100, Wing.Params.ChordLength, Wing.Params.Thickness);

        for (auto &Point : ProfileVertices.GetPoints()) {
            Point.x = (Point.x+x) * Scale;
            Point.y = (Point.y+y) * Scale;
        }

        Draw::RenderPointSet(Dummy(ProfileVertices), StrongStrokeStyle);
    }
}

TPointer<FPlaneFactory> SetupDefaultPlane() {
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