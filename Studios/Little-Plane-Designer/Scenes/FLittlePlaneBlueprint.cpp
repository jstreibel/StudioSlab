//
// Created by joao on 11/21/25.
//

#include "FLittlePlaneBlueprint.h"
#include "../Physics/Materials.h"
#include "../Physics/Foils/NACA2412.h"
#include "Utils/Angles.h"

void FLittlePlaneBlueprint::Tick(Seconds ElapsedTime) {

}

void FLittlePlaneBlueprint::TogglePause() {

}

void FLittlePlaneBlueprint::Draw(const Graphics::FDrawInput&) {

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