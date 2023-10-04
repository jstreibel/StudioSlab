//
// Created by joao on 13/05/2020.
//

#ifndef MOLEKUL_PLAY_MOLECULESPACEHASH_H
#define MOLEKUL_PLAY_MOLECULESPACEHASH_H

#include "FixedSizeMoleculeContainer.h"


namespace MolecularDynamics {

    class MoleculeSpaceHash {
    public:
        MoleculeSpaceHash(int linSubdivs, Real L);

        void ComputeHash(const Graphics::PointContainer &v_q,
                         const Graphics::PointContainer &v_p,
                         Graphics::PointContainer &dpdt);

        Real totalLength() const;

        FixedSizeMoleculeContainer &get(int i, int j);

        std::vector<FixedSizeMoleculeContainer::VerletPointTriple *> gather(int i, int j);


        const int n;
        const Real L, l;
        std::vector<FixedSizeMoleculeContainer> hashSpace;

    };

}


#endif //MOLEKUL_PLAY_MOLECULESPACEHASH_H
