//
// Created by joao on 10/24/25.
//

#ifndef STUDIOSLAB_APP_H
#define STUDIOSLAB_APP_H


#include "Application.h"
#include "Camera.h"
#include "DebugDraw.h"
#include "Physics/FLittlePlane.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"
#include "Graphics/SFML/Graph.h"

#include "box2d/box2d.h"
#include "Core/Tools/Log.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Physics/Terrain.h"
#include "../../Lib/Graphics/Interfaces/IDrawable.h"

#define DeclarePointset(Name) TPointer<Math::FPointSet> Name = New<Math::FPointSet>();

inline double DegToRad(const double ang) { return ang * M_PI / 180.0;}
inline double RadToDeg(const double ang) { return ang * 180.0 / M_PI;}

class FLittlePlaneDesignerApp final : public FApplication {

public:
    FLittlePlaneDesignerApp(int argc, const char* argv[]);

    ~FLittlePlaneDesignerApp() override;

    bool NotifyRender(const Graphics::FPlatformWindow& PlatformWindow) override;

    bool NotifyKeyboard(Graphics::EKeyMap key, Graphics::EKeyState state, Graphics::EModKeys modKeys) override;

private:
    b2WorldId World;

    TPointer<Graphics::FGUIContext> GUIContext;

    const int PlotsHeight = 400;
    Vector<TPointer<Graphics::FPlot2DWindow>> Plots;

    void SetupMonitors();

    void SetupPlane();
    void SetupTerrain();
    void OnStart() override;

    void HandleInputs(const Graphics::FKeyboardState& KeyboardState);

    void UpdateGraphs() const;
    void DoPhysicsDraw() const;
    void RenderSimData(const Graphics::FPlatformWindow& PlatformWindow);

    void StepSimulation() const;

    FCamera Camera;
    Int WinHeight, WinWidth;

    DeclarePointset(ForcesTimeSeries)

    DeclarePointset(EnergyTotalTimeSeries)
    DeclarePointset(EnergyPotentialTimeSeries)
    DeclarePointset(EnergyKineticLinearTimeSeries)
    DeclarePointset(EnergyKineticAngularTimeSeries)

    DeclarePointset(CurrentLiftPolar)
    DeclarePointset(CurrentDragPolar)
    DeclarePointset(CurrentTorquePolar)

    TPointer<LegacyGLDebugDraw> DebugDraw_LegacyGL;
    bool b_IsRunning;
    TPointer<FLittlePlane> LittlePlane;
    TPointer<FTerrain> Terrain;
    TPointer<Graphics::IDrawable> PlaneStats;

    TList<TPointer<Graphics::IDrawable>> Drawables;

};

#endif //STUDIOSLAB_APP_H