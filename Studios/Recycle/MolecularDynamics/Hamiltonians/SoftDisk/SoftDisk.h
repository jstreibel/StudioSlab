#ifndef MOLDYN_H
#define MOLDYN_H

#include "Hamiltonians/NewtonMechanics.h"
#include "Hamiltonians/Types.h"
#include "IO/KeyPressListener.h"
#include "Hamiltonians/Hash/MoleculeSpaceHash.h"
#include "SoftDiskParams.h"

class SoftDisk : public NewtonMechanics, public KeyPressListener{
public:
    SoftDisk() = default;

    void KeyPressed(sf::Event::KeyEvent &keyEvent, int value) override;

protected:
    Real U(const Point2D &q1, const Point2D &q2) override;
    Point2D mdUdr (const Point2D &q1, const Point2D &q2) override;
};



#endif // MOLDYN_H
