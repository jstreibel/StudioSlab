#ifndef MOLDYN_H
#define MOLDYN_H

#include "Hamiltonians/NewtonMechanics.h"
#include "Hamiltonians/Types.h"

class SoftDisk : public NewtonMechanics {
public:
    SoftDisk() = default;

protected:
    Real U(const Point2D &q1, const Point2D &q2) override;
    Point2D mdUdr (const Point2D &q1, const Point2D &q2) override;
};



#endif // MOLDYN_H
