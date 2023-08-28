//
// Created by joao on 05/08/2021.
//

#ifndef MOLEKUL_PLAY_FIXEDSIZEMOLECULECONTAINER_H
#define MOLEKUL_PLAY_FIXEDSIZEMOLECULECONTAINER_H

#include "Utils/Types.h"
#include "HashParams.h"


#define get_q(triple) (*(triple.first.first))
#define get_p(triple) (*(triple.first.second))
#define get_dpdt(triple) (*(triple.second))


class FixedSizeMoleculeContainer{
public:
    typedef std::pair<const Point2D*, const Point2D*> PointRefPair;
    typedef std::pair<PointRefPair, Point2D*> VerletPointTriple;

public:
    FixedSizeMoleculeContainer() = default;

    void reset();
    void add(VerletPointTriple triple);
    inline int getOccupation() const {return occupation; }

    VerletPointTriple &getPair(unsigned n) { return triples[n]; }

private:
    VerletPointTriple triples[HASH_CONTAINER_SIZE];
    int occupation = 0;
};

#endif //MOLEKUL_PLAY_FIXEDSIZEMOLECULECONTAINER_H
