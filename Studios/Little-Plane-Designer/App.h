//
// Created by joao on 10/24/25.
//

#ifndef STUDIOSLAB_APP_H
#define STUDIOSLAB_APP_H


#include "Application.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Physics/FLPDPhysicsEngine.h"
#include "Plane/FPlaneFactory.h"
#include "Scenes/Camera.h"
#include "Scenes/Scene.h"

using namespace Slab;

class FLittlePlaneDesignerApp final : public FApplication {

public:
    FLittlePlaneDesignerApp(int argc, const char* argv[]);

    bool NotifyRender(const Graphics::FPlatformWindow& PlatformWindow) override;

    bool NotifyKeyboard(Graphics::EKeyMap key, Graphics::EKeyState state, Graphics::EModKeys modKeys) override;

private:
    FTrackerCamera Camera;

    FLittlePlaneDesignerPhysicsEngine PhysicsEngine;

    TPointer<FLittlePlane> Plane;

    TPointer<IInputStateReader> Controller;
    TPointer<Graphics::IViewProvider> View;
    TPointer<Graphics::FGUIContext> GUIContext;

    void OnStart() override;
    static TPointer<FLittlePlane> SetupPlane(b2WorldId);

    Vector<TPointer<Graphics::IDrawable2D>> Drawables;
    TPointer<IScene> CurrentScene;

};

#endif //STUDIOSLAB_APP_H