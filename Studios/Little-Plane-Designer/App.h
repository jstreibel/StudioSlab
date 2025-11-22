//
// Created by joao on 10/24/25.
//

#ifndef STUDIOSLAB_APP_H
#define STUDIOSLAB_APP_H


#include "Application.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Plane/FPlaneFactory.h"
#include "Scenes/BigHill.h"
#include "Scenes/Camera.h"
#include "Scenes/FLittlePlaneBlueprint.h"
#include "Scenes/Scene.h"

using namespace Slab;

class FLittlePlaneDesignerApp final : public FApplication {

public:
    FLittlePlaneDesignerApp(int argc, const char* argv[]);

    bool NotifyRender(const Graphics::FPlatformWindow& PlatformWindow) override;

    bool NotifyKeyboard(Graphics::EKeyMap key, Graphics::EKeyState state, Graphics::EModKeys modKeys) override;

private:
    void OnStart() override;
    static FPlaneFactory SetupPlane();

    void SetSceneBigHill();
    void SetSceneBlueprint();

    TPointer<IInputStateReader> Controller;

    TPointer<FBigHill> BigHill;
    TPointer<FLittlePlaneBlueprint> BlueprintScene;

    TPointer<IScene> CurrentScene;

    TPointer<Graphics::FGUIContext> GUIContext;
};

#endif //STUDIOSLAB_APP_H