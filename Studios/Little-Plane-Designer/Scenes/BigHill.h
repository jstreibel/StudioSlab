//
// Created by joao on 11/20/25.
//

#ifndef STUDIOSLAB_BIGHILL_H
#define STUDIOSLAB_BIGHILL_H

#include "Scene.h"
#include "Camera.h"
#include "../Physics/Terrain.h"
#include "../Plane/FPlaneFactory.h"
#include "../Physics/FLPDPhysicsEngine.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Utils/List.h"

using namespace Slab;

#define DeclarePointset(Name) TPointer<Math::FPointSet> Name = New<Math::FPointSet>();

class FBigHill final : public IScene, public IInputStateReader {
public:
    explicit FBigHill(FPlaneFactory PlaneFactory);

    void Startup(const Graphics::FPlatformWindow&) override;
    void Tick(Seconds ElapsedTime) override;

    void Draw(const Graphics::FDrawInput&) override;

    void HandleInputState(FInputState) override;


    void TogglePause() override;

private:
    void SetupScenario();

    TPointer<FTrackerCamera> Camera = New<FTrackerCamera>();
    TPointer<IInputStateReader> Controller;

    TPointer<FLittlePlaneDesignerPhysicsEngine> PhysicsEngine = New<FLittlePlaneDesignerPhysicsEngine>();

    TPointer<FLittlePlane> Plane;
    TPointer<FTerrain> Terrain;

    TList<TPointer<IUpdateable>> Updateables;
    TList<TPointer<Graphics::IDrawable2D>> Drawables;

    bool b_IsRunning = false;
    const int PlotsHeight = 400;
    Vector<TPointer<Graphics::FPlot2DWindow>> Plots;
    TPointer<Graphics::FGUIContext> GUIContext;


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