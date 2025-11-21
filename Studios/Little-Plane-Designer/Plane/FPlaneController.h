//
// Created by joao on 11/20/25.
//

#ifndef STUDIOSLAB_FPLANECONTROLLER_H
#define STUDIOSLAB_FPLANECONTROLLER_H
#include "FLittlePlane.h"
#include "../Interfaces.h"


class FPlaneController : public IInputStateReader
{
public:
    explicit FPlaneController(TPointer<FLittlePlane> Plane);

    void HandleInputState(FInputState) override;

private:
    TPointer<FLittlePlane> Plane;
};


#endif //STUDIOSLAB_FPLANECONTROLLER_H