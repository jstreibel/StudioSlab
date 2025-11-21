//
// Created by joao on 11/20/25.
//

#ifndef STUDIOSLAB_BIGHILL_H
#define STUDIOSLAB_BIGHILL_H

#include "Scene.h"
#include "Camera.h"
#include "../Physics/Terrain.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Utils/List.h"

using namespace Slab;

#define DeclarePointset(Name) TPointer<Math::FPointSet> Name = New<Math::FPointSet>();

class FBigHill final : public IScene {
public:
    explicit FBigHill(const b2WorldId World) : World(World) {}

    void Startup(const Graphics::FPlatformWindow&) override;
    void Tick(Miliseconds ElapsedTime) override;
    void Draw(const Graphics::FDraw2DParams&) override;

    void TogglePause() override;

private:
    void SetupScenario();

    TPointer<Graphics::FGUIContext> GUIContext;

    const int PlotsHeight = 400;
    Vector<TPointer<Graphics::FPlot2DWindow>> Plots;

    b2WorldId World;

    FTrackerCamera Camera;

    bool b_IsRunning = false;
    TPointer<FTerrain> Terrain;

    TList<TPointer<IDrawable2D>> Drawables;


    /*
    void UpdateGraphs() const;
    void DoPhysicsDraw() const;
    void SetupMonitors();

    DeclarePointset(ForcesTimeSeries)

    DeclarePointset(EnergyTotalTimeSeries)
    DeclarePointset(EnergyPotentialTimeSeries)
    DeclarePointset(EnergyKineticLinearTimeSeries)
    DeclarePointset(EnergyKineticAngularTimeSeries)

    DeclarePointset(CurrentLiftPolar)
    DeclarePointset(CurrentDragPolar)
    DeclarePointset(CurrentTorquePolar)
    TPointer<LegacyGLDebugDraw> DebugDraw_LegacyGL;
    TPointer<IDrawable2D> PlaneStats;
    */
};

#endif //STUDIOSLAB_BIGHILL_H