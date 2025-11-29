//
// Created by joao on 11/20/25.
//

#ifndef STUDIOSLAB_FLITTLEPLANEDESIGNERPHYSICSENGINE_H
#define STUDIOSLAB_FLITTLEPLANEDESIGNERPHYSICSENGINE_H

#include "box2d/box2d.h"
#include "Utils/Pointer.h"

#include "../Plane/FLittlePlane.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"


struct FLittlePlaneDesignerPhysicsEngine final : IUpdateable, Graphics::IDrawable2D
{
    explicit FLittlePlaneDesignerPhysicsEngine();

    void Tick(Seconds ElapsedTime) override;
    void TogglePause() override;
    void ToggleDraw();

    void SetPlane(const TPointer<FLittlePlane>& NewPlane) { this->Plane = NewPlane; }

    b2WorldId GetWorld() const { return World; }

    void Draw(const Graphics::FDraw2DParams&) override;

    TPointer<const FLittlePlane> GetPlane() const;

private:
    // TPointer<Graphics::FImGuiContext> ImGuiContext;

    LegacyGLDebugDraw DebugDraw;
    TPointer<FLittlePlane> Plane;
    b2WorldId World;
    bool b_IsRunning = true;
    bool b_DrawPhysics = false;

};


#endif //STUDIOSLAB_FLITTLEPLANEDESIGNERPHYSICSENGINE_H