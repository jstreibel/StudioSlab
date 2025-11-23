//
// Created by joao on 11/21/25.
//

#ifndef STUDIOSLAB_FLITTLEPLANEBLUEPRINT_H
#define STUDIOSLAB_FLITTLEPLANEBLUEPRINT_H

#include "Scene.h"
#include "../Plane/FPlaneFactory.h"


class FLittlePlaneBlueprint final : public IScene, public IInputStateReader
{
public:
    explicit FLittlePlaneBlueprint();

    void Tick(Seconds ElapsedTime) override;
    void TogglePause() override;
    void Draw(const Graphics::FDrawInput&) override;
    void Startup(const Graphics::FPlatformWindow&) override;
    auto BuildPlane(b2WorldId) -> TPointer<FLittlePlane>;
    auto GetPlaneFactory() -> TPointer<FPlaneFactory>;

    void HandleInputState(FInputState) override;
    bool NotifyKeyboard(Graphics::EKeyMap key, Graphics::EKeyState state,
        Graphics::EModKeys modKeys) override;

private:
    TPointer<FPlaneFactory> PlaneFactory;
    TPointer<Graphics::OpenGL::FWriterOpenGL> Writer;

    int Proportion = 40; // Scale is 1:Proportion

    static void DrawBlueprint();
    void DrawPlane();

};


#endif //STUDIOSLAB_FLITTLEPLANEBLUEPRINT_H