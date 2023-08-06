//
// Created by joao on 13/05/2020.
//

#ifndef MOLEKUL_PLAY_MOLECULESPACEHASH_H
#define MOLEKUL_PLAY_MOLECULESPACEHASH_H

#include "FixedSizeMoleculeContainer.h"


class MoleculeSpaceHash {
public:
    MoleculeSpaceHash(int linSubdivs, Real L);
    void ComputeHash(const PointContainer &v_q, const PointContainer &v_p, PointContainer &dpdt);

    Real totalLength() const;

    FixedSizeMoleculeContainer &get(int i, int j);
    std::vector<FixedSizeMoleculeContainer::VerletPointTriple*> gather(int i, int j);


    const int n;
    const Real L, l;
    std::vector<FixedSizeMoleculeContainer> hashSpace;

};


#endif //MOLEKUL_PLAY_MOLECULESPACEHASH_H
