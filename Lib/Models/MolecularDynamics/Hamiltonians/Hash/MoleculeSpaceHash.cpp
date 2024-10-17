//
// Created by joao on 13/05/2020.
//

#include "MoleculeSpaceHash.h"

namespace MolecularDynamics {

    MoleculeSpaceHash::MoleculeSpaceHash(int linSubdivs, Real L) : n(linSubdivs), L(L), l(L / n), hashSpace(n * n) {
        if (n < 3) throw "Hash space linear subdivisions must be >= 3.";
    }

    void MoleculeSpaceHash::ComputeHash(const Graphics::PointContainer &v_q, const Graphics::PointContainer &v_p, Graphics::PointContainer &dpdt) {
        for (auto &c: hashSpace) c.reset();

        for (auto k = 0; k < v_q.size(); ++k) {
            const Graphics::Point2D &q = v_q[k];

            double x = q.x - L * floor(q.x / L + .5) + .5 * L,
                    y = q.y - L * floor(q.y / L + .5) + .5 * L;

            int i = floor(x / l),
                    j = floor(y / l);

            assert(!(i < 0 || i >= n || j < 0 || j >= n));

            hashSpace[i + j * n].add({{&v_q[k], &v_p[k]}, &dpdt[k]});
        }
    }

    Vector<FixedSizeMoleculeContainer::VerletPointTriple *> MoleculeSpaceHash::gather(int i, int j) {
        int E = i + 1, S = j + 1;
        if (i == n - 1) E = 0;
        if (j == n - 1) S = 0;

        FixedSizeMoleculeContainer everybody[4] = {get(i, j),
                                                   get(i, S),
                                                   get(E, S),
                                                   get(E, j)};

        Vector<FixedSizeMoleculeContainer::VerletPointTriple *> a;
        for (auto &container: everybody) {
            for (auto k = 0; k < container.getOccupation(); ++k) {
                auto &triple = container.getPair(k);
                a.emplace_back(&triple);
            }
        }

        return a;
    }

    FixedSizeMoleculeContainer &MoleculeSpaceHash::get(int i, int j) {
        return hashSpace[i + j * n];
    }

    Real MoleculeSpaceHash::totalLength() const {
        return L;
    }

}