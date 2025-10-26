//
// Created by joao on 10/24/25.
//

#ifndef STUDIOSLAB_APP_H
#define STUDIOSLAB_APP_H


#include "Application.h"
#include "DebugDraw.h"
#include "FLittlePlane.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"
#include "Graphics/SFML/Graph.h"

#include "box2d/box2d.h"
#include "Core/Tools/Log.h"
#include "Graphics/Plot2D/Plot2DWindow.h"

#define DeclarePointset(Name) TPointer<Math::PointSet> Name = New<Math::PointSet>();

auto DegToRad(const auto ang) { return ang * M_PI / 180.0;}
auto RadToDeg(const auto ang) { return ang * 180.0 / M_PI;}

class FLittlePlaneDesignerApp final : public FApplication {
    TPointer<LegacyGLDebugDraw> DebugDraw_LegacyGL;

    float ViewWidth = 12.0;

    bool b_IsRunning = false;

    TPointer<FLittlePlane> LittlePlane;
public:
    void ComputeAndApplyForces() const;
    void StepSimulation() const;
    bool NotifyRender(const Graphics::FPlatformWindow& PlatformWindow) override;

protected:
    b2WorldId World;

    TPointer<Graphics::FGUIContext> GUIContext;

    const int PlotsHeight = 400;
    Vector<TPointer<Graphics::FPlot2DWindow>> Plots;

    DeclarePointset(ForcesTimeSeries)

    DeclarePointset(EnergyTotalTimeSeries)
    DeclarePointset(EnergyPotentialTimeSeries)
    DeclarePointset(EnergyKineticLinearTimeSeries)
    DeclarePointset(EnergyKineticAngularTimeSeries)

    DeclarePointset(CurrentLiftPolar)
    DeclarePointset(CurrentDragPolar)
    DeclarePointset(CurrentTorquePolar)

    void SetupMonitors();

    void OnStart() override;

    void DoDebugDraw() const;

    void Monitor(const Graphics::FPlatformWindow& PlatformWindow);

public:
    bool NotifyKeyboard(Graphics::EKeyMap key, Graphics::EKeyState state, Graphics::EModKeys modKeys) override;

    FLittlePlaneDesignerApp(const int argc, const char* argv[]);

    ~FLittlePlaneDesignerApp() override;


};

#endif //STUDIOSLAB_APP_H