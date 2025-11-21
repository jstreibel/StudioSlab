//
// Created by joao on 11/20/25.
//

#ifndef STUDIOSLAB_FLITTLEPLANEDESIGNERPHYSICSENGINE_H
#define STUDIOSLAB_FLITTLEPLANEDESIGNERPHYSICSENGINE_H

#include "box2d/box2d.h"
#include "Utils/Pointer.h"

#include "../Plane/FLittlePlane.h"


struct FLittlePlaneDesignerPhysicsEngine : IUpdateable
{
    FLittlePlaneDesignerPhysicsEngine();

    void Tick(Miliseconds ElapsedTime) override;
    void TogglePause() override;

    void SetPlane(const TPointer<FLittlePlane>& NewPlane) { this->Plane = NewPlane; }

    b2WorldId GetWorld() const { return World; }

private:
    TPointer<FLittlePlane> Plane;
    b2WorldId World;
    bool b_IsRunning = true;

};


#endif //STUDIOSLAB_FLITTLEPLANEDESIGNERPHYSICSENGINE_H