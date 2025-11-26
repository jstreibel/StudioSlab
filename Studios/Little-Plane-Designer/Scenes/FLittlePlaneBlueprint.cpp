//
// Created by joao on 11/21/25.
//

#include "FLittlePlaneBlueprint.h"
#include "../Physics/Materials.h"
#include "../Physics/PolygonMassProperties.h"
#include "../Physics/Foils/NACA2412.h"
#include "Core/Tools/Log.h"
#include "Utils/Angles.h"

#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"
#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/SFML/Graph.h"
#include "Math/Geometry/Geometry.h"

constexpr auto ArchD_height = 0.457; //m
constexpr auto ArchD_width = 0.610; // m
constexpr auto PaperHeight = ArchD_height/2;
constexpr auto PaperWidth = PaperHeight * 16 / 9.;
constexpr auto RegionHeight_2 = PaperHeight * 1.2 / 2.0;

const auto OuterX = floorf(100*PaperWidth/2)/100;
const auto OuterY = floorf(100*PaperHeight/2)/100;
const auto InnerX = OuterX - 0.01;
const auto InnerY = OuterY - 0.005;

constexpr auto Width_StrongStroke = 2.5;
constexpr auto Width_RegularStroke = 1.5;
constexpr auto Width_WeakStroke = 1.;

const auto BackgroundColor = Graphics::WoodLight;
const auto BorderColor = Graphics::WhiteBlueprintStroke;
const auto LineColor = Graphics::GreyBlue*0.75;
const auto StrokeColor = Graphics::White;
const auto BlueprintColor = Graphics::BlueprintLight;

Graphics::PlotStyle StrongStrokeStyle{StrokeColor, Graphics::LineLoop, false, Graphics::Nil, Width_StrongStroke};
Graphics::PlotStyle RegularStrokeStyle{StrokeColor, Graphics::LineLoop, false, Graphics::Nil, Width_RegularStroke};
Graphics::PlotStyle WeakStrokeStyle{StrokeColor, Graphics::LineLoop, false, Graphics::Nil, Width_WeakStroke};

namespace Draw = Graphics::OpenGL::Legacy;


FLittlePlaneBlueprint::FLittlePlaneBlueprint() {
    // auto FontFile = Core::Resources::BuiltinFonts::EngineerHand();
    auto FontFile = Core::Resources::BuiltinFonts::Y145m2009();

    Writer = Slab::New<Graphics::OpenGL::FWriterOpenGL>(FontFile, 36);
}

void FLittlePlaneBlueprint::Tick(Seconds ElapsedTime) {

}

void FLittlePlaneBlueprint::TogglePause() {

}

void FLittlePlaneBlueprint::SetupAnnotations() {
    NumAnnotations = 0;
    NumGlobalAnnotations = 0;
}

void FLittlePlaneBlueprint::Draw(const Graphics::FDrawInput& Input) {
    SetupAnnotations();

    Input.Window.Clear(BackgroundColor);

    fix WinHeight = Input.Window.GetHeight();
    fix WinWidth = Input.Window.GetWidth();
    fix AspectRatio = static_cast<double>(WinWidth)/WinHeight;

    const auto RegionWidth_2 = RegionHeight_2 * AspectRatio;
    Draw::SetupOrtho({-RegionWidth_2, RegionWidth_2, -RegionHeight_2, RegionHeight_2});

    Writer->Reshape(WinWidth, WinHeight);
    fix Region = Graphics::RectR{-RegionWidth_2, RegionWidth_2, -RegionHeight_2, RegionHeight_2};
    fix VP = Graphics::RectI{0, WinWidth, 0, WinHeight};
    Writer->SetPenPositionTransform([&](const Graphics::Point2D Pt) {
        return Graphics::FromSpaceToViewportCoord(Pt, Region, VP);
    });
    GlyphHeight = .5/Proportion * Graphics::FromViewportToSpaceCoord({0., Writer->GetFontHeightInPixels()}, Region, VP).y;

    Graphics::OpenGL::Legacy::PushLegacyMode();
    DrawBlueprint();

    DrawPlane();
    Graphics::OpenGL::Legacy::RestoreFromLegacyMode();

    Writer->Write(ToStr("1:%i", Proportion), {InnerX-0.01, -InnerY+0.0025});
}

void FLittlePlaneBlueprint::Startup(const Graphics::FPlatformWindow&) {
}

void FLittlePlaneBlueprint::HandleInputState(FInputState) {

}

bool FLittlePlaneBlueprint::NotifyKeyboard(Graphics::EKeyMap key, Graphics::EKeyState state, Graphics::EModKeys modKeys) {
    return false;
}

void FLittlePlaneBlueprint::AddPartAnnotation(const Str& Annotation, Math::Real2D ItemLocation) {
    const int Sx = +(2*(NumAnnotations%2) - 1);
    const int Sy = -(2*(NumAnnotations/2) - 1);

    const Graphics::Point2D AnnotationLocation = {Sx*InnerX/2, Sy*InnerY/2};

    fix d = GlyphHeight;
    Writer->Write(Annotation, AnnotationLocation+Graphics::Point2D{d, -d});

    Draw::SetupLegacyGL();
    Draw::SetColor(Graphics::White);

    Draw::DrawLine(ItemLocation, AnnotationLocation.ToReal2D(), StrokeColor, .5);

    ++NumAnnotations;
}

void FLittlePlaneBlueprint::AddGlobalCharacteristicAnnotation(const Str& Annotation) {
    constexpr auto MaxLines = 3;
    Writer->Write(Annotation, {InnerX - 40*GlyphHeight, -InnerY + 4*(MaxLines-NumGlobalAnnotations)*GlyphHeight});
    ++NumGlobalAnnotations;
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

void RenderCOM(const Math::Point2D COM) {

    constexpr auto Radius = PaperHeight / 45.;
    auto Circle = Math::Geometry::FCircle{COM, Radius};
    auto FullCirclePoints = Circle.GetPoints();
    Circle.EndAngle = M_PI/2.;
    auto FirstQuarter = COM + Circle.GetPoints();
    Circle.BeginAngle = M_PI;
    Circle.EndAngle = 3.*M_PI/2.;
    auto LastQuarter = COM + Circle.GetPoints();

    Draw::RenderPointSet(Dummy(FullCirclePoints), WeakStrokeStyle);
    auto FilledStroke = WeakStrokeStyle;
    FilledStroke.setPrimitive(Graphics::TriangleFan);
    FilledStroke.fillColor = WeakStrokeStyle.lineColor;
    FilledStroke.lineColor.a = 0.25;
    FilledStroke.filled = true;

    Draw::RenderPointSet(Dummy(FirstQuarter), FilledStroke);
    Draw::RenderPointSet(Dummy(LastQuarter), FilledStroke);

    constexpr auto XTrap = Radius*1.25;
    Draw::DrawLine({COM.x - XTrap, COM.y}, {COM.x + XTrap, COM.y}, StrokeColor, Width_WeakStroke);
    Draw::DrawLine({COM.x, COM.y - XTrap}, {COM.x, COM.y + XTrap}, StrokeColor, Width_WeakStroke);

}

void FLittlePlaneBlueprint::DrawPlane() {
    fix Scale = 1./static_cast<double>(Proportion);

    Math::Point2D COM = {0,0};
    Real64 Mass = 0;
    Real64 HullMass = 0;
    Real64 WingMass = 0;

    const Math::Real2D SideViewOrigin = {0, -3*InnerY/4};
    const Math::Real2D TopViewOrigin = {0,  +1*InnerY/4};

    for (const auto &Part : GetPlaneFactory()->BodyPartDescriptors) {
        auto LeftViewPoints = FBodyPartRenderer{Part}.GetLeftView();
        auto TopViewPoints = FBodyPartRenderer{Part}.GetTopView();

        auto COMResult = Math::Geometry::ComputeCentroid(LeftViewPoints);
        if (COMResult.IsFailure()) continue;

        const auto Volume = Math::Geometry::ComputeArea(LeftViewPoints) * Part.Depth;

        fix MyCOM = COMResult.Value();
        fix MyMass = Volume * Part.Density;

        COM += MyCOM * MyMass;
        Mass += MyMass;
        HullMass += MyMass;

        Draw::RenderPointSet(Dummy((LeftViewPoints*=Scale).Translate(SideViewOrigin)), StrongStrokeStyle);
        Draw::RenderPointSet(Dummy((TopViewPoints*=Scale).Translate(TopViewOrigin)), StrongStrokeStyle);
    }

    static bool FirstRun = true;
    for (const auto &Wing : GetPlaneFactory()->WingDescriptors) {
        auto SideViewPoints = FWingDescriptorRenderer {Wing}. GetLeftView();
        auto TopViewPoints  = FWingDescriptorRenderer {Wing}. GetTopView();

        if (auto PolygonValidationResult = Math::Geometry::ValidatePolygon(SideViewPoints); !PolygonValidationResult) {
            Core::Log::Error(ToStr("Bad airfoil profile: \"%s\"", PolygonValidationResult.ToString().c_str()));
            continue;
        }
        fix WingMassProperties = Wing.ComputeMassProperties();

        fix MyCOM = WingMassProperties.Centroid;
        fix MyMass = WingMassProperties.Mass;

        COM += MyCOM * MyMass;
        Mass += MyMass;
        WingMass += MyMass;

        if (FirstRun) {
            Core::Log::Info(ToStr("Wing mass: %.2fkg", MyMass));
        }

        Draw::RenderPointSet((SideViewPoints * Scale).Translate(SideViewOrigin), StrongStrokeStyle);
        Draw::RenderPointSet((TopViewPoints * Scale).Translate(TopViewOrigin), StrongStrokeStyle);

        AddPartAnnotation(Wing.Airfoil->GetName(), MyCOM*Scale + SideViewOrigin);
    }
    FirstRun = false;

    RenderCOM(COM * Scale / Mass + SideViewOrigin);
    RenderCOM(Math::Real2D{COM.x, 0.0} * Scale / Mass + TopViewOrigin);


    AddGlobalCharacteristicAnnotation(ToStr("Total mass: %.2fkg", Mass));
    AddGlobalCharacteristicAnnotation(ToStr("Hull mass: %.2fkg", HullMass));
    AddGlobalCharacteristicAnnotation(ToStr("Wings mass: %.2fkg", WingMass));
}

TPointer<FPlaneFactory> SetupDefaultPlane() {
    auto Factory = New<FPlaneFactory>();

    Factory.get()
    ->SetPosition({161.5f, 41.f})
    .SetRotation(DegToRad(0.0f))
    .AddBodyPart({
        .Density = LightMaterialDensity,
        .Length = 4.0f,
        .Height = 0.5,
        .Depth = 0.8f,
    })
    .AddBodyPart({
        .Density = HeavyRockDensity,
        .Length = 0.4,
        .Height = 0.15,
        .Depth = 0.25,
        .xOffset = -1.85
    })
    .AddBodyPart({
        .Density = LightMaterialDensity,
        .Length = 0.4,
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