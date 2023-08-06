//
// Created by joao on 05/08/2021.
//

#ifndef MOLEKUL_PLAY_NEWTONMECHANICS_H
#define MOLEKUL_PLAY_NEWTONMECHANICS_H


#include "Hamiltonians/Types.h"
#include "Hamiltonians/Hash/MoleculeSpaceHash.h"


class NewtonMechanics {
public:
    NewtonMechanics();
    ~NewtonMechanics();

    Real computeEnergy(const PointContainer &v_q, PointContainer &v_p);

    void operator() ( const PointContainer & q ,
                      const PointContainer & p ,
                      PointContainer & dpdt , double /* t */);


    bool *flippedSides;
    Real dissipation;

protected:
    virtual void applyBoundaryConditions(PointContainer & v_q);
    virtual Real U(const Point2D &q1, const Point2D &q2) { return 0; };
    virtual Point2D mdUdr (const Point2D &q1, const Point2D &q2) = 0;

private:
    MoleculeSpaceHash spaceHash;
};


#endif //MOLEKUL_PLAY_NEWTONMECHANICS_H
