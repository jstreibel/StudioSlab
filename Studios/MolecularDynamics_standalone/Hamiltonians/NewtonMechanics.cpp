//
// Created by joao on 05/08/2021.
//

#include <sstream>
#include "NewtonMechanics.h"
#include "NewtonMechanicsParams.h"
#include "Hash/HashParams.h"


#define USE_NEW_EXPERIMENTAL_IMPLEMENTATION false


NewtonMechanics::NewtonMechanics() : dissipation(DISSIPATION_FACTOR),
                                    spaceHash(HASH_SUBDIVS, Real(SPACE_L)),
                                    flippedSides(new bool[N_MOLS]) {
    if(spaceHash.totalLength() != SPACE_L){
        std::ostringstream ss;

        ss << "Hashspace inconsistency. Hashspace total width is " << spaceHash.totalLength()
           << " while sim space is " << SPACE_L;

        std::cout << ss.str();

        throw ss.str().c_str();

    } else if(spaceHash.l <= CUTOFF_RADIUS) {
        std::ostringstream ss;

        ss << "Hashspace inconsistency. Hashspace box length is " << spaceHash.l
           << " while molecule cutoff radius is " << CUTOFF_RADIUS;

        std::cout << ss.str();
        throw ss.str().c_str();
    }

    for(auto i=0; i<N_MOLS; ++i)
        flippedSides[i] = false;
}

NewtonMechanics::~NewtonMechanics() {
    // TODO delete bools
    // delete [] flippedSides;
}


void NewtonMechanics::applyBoundaryConditions(PointContainer & v_q) {
    const Real hw = SPACE_L * .5;
    int i = 0;
    for (auto &q : v_q) {
        if (q.x < -hw || q.x >= hw) {
            q.x -= SPACE_L* floor(q.x/SPACE_L + .5);
            flippedSides[i] = true;
        }

        if (q.y < -hw || q.y >= hw) {
            q.y -= SPACE_L* floor(q.y/SPACE_L + .5);;
            flippedSides[i] = true;
        }

        i++;
    }
}


Real NewtonMechanics::computeEnergy(const PointContainer &v_q, PointContainer &v_p) {
    Real K = .0;
    for (auto &p : v_p)
        K += p.lengthSqr();
    K *= .5;

    const size_t N = v_q.size();
    Real _U = 0.0;
    for (size_t i = 0; i < N - 1; i++) {
        const Point2D &q1 = v_q[i];
        for (size_t j = i; j < N; j++) {
            const Point2D &q2 = v_q[j];

            _U += U(q1, q2);
        }
    }

    return K + _U;
}

void
NewtonMechanics::operator()(const PointContainer &v_q, const PointContainer &v_p, PointContainer &v_dpdt,
                            double) {
    // u_1 (single molecule potential)
    #if DISSIPATE
    for (int i = 0; i < v_q.size(); i++) {
        auto &q = v_q[i];
        auto &p = v_p[i];
        auto &F = v_dpdt[i];

        F.x = -dissipation * p.x;
        F.y = -dissipation * p.y;
    }
    #else
    for(auto &F : v_dpdt) F = {0, 0};
    #endif

    applyBoundaryConditions(const_cast<PointContainer &>(v_q));

    #if USE_NEW_EXPERIMENTAL_IMPLEMENTATION
    spaceHash.ComputeHash(v_q, v_p, v_dpdt);

    for (int i = 0; i < spaceHash.n; ++i) {
        for (int j = 0; j < spaceHash.n; ++j) {
            auto triples = spaceHash.gather(i, j);
            auto occ = triples.size();

            if(occ==0) continue;

            {
                for (size_t k = 0; k < (occ - 1); ++k) {
                    auto &triple1 = *(triples[k]);
                    const auto &q1 = get_q(triple1);
                    auto &F1 = get_dpdt(triple1);
                    for (size_t l = k + 1; l < occ; ++l) {
                        auto &triple2 = *(triples[l]);
                        const auto &q2 = get_q(triple2);
                        auto &F2 = get_dpdt(triple2);
                        const Point2D F = mdUdr(q1, q2);
                        F1 = F1 + F;
                        F2 = F2 - F;
                    }
                }
            }
        }
    }

    #elif USE_NEW_EXPERIMENTAL_IMPLEMENTATION && false

    // u_2 (molecule pair potential)
    for (int i = 0; i < spaceHash.n; ++i) {
        for (int j = 0; j < spaceHash.n; ++j) {
            FixedSizeMoleculeContainer &center = spaceHash.get(i, j);

            const int occ = center.getOccupation();
            if (occ == 0) continue;

            // Interaction between molecules within the same container
            {
                for (size_t k = 0; k < (occ - 1); ++k) {
                    auto &triple1 = center.getPair(k);

                    const auto &q1 = get_q(triple1);
                    auto       &F1 = get_dpdt(triple1);

                    for (size_t l = k + 1; l < occ; ++l) {
                        auto &triple2 = center.getPair(l);

                        const auto &q2 = get_q(triple2);
                        auto &F2 = get_dpdt(triple2);

                        const Point2D F = mdUdr(q1, q2);

                        F1 = F1 + F;
                        F2 = F2 - F;
                    }
                }
            }


            // Interaction between current container and neighbor containers
            {
                int E = i + 1, S = j + 1;
                if (i == spaceHash.n - 1) E = 0;
                if (j == spaceHash.n - 1) S = 0;
                FixedSizeMoleculeContainer neighbours[3] = {spaceHash.get(i, S),
                                                            spaceHash.get(E, S),
                                                            spaceHash.get(E, j)};
                for (size_t k = 0; k < occ; ++k) {
                    auto &triple1 = center.getPair(k);

                    const auto &q1 = get_q(triple1);
                    auto       &F1 = get_dpdt(triple1);

                    for (FixedSizeMoleculeContainer &neighbor : neighbours) {
                        int occ2 = neighbor.getOccupation();
                        for (size_t l = 0; l < occ2; ++l) {
                            auto &triple2 = neighbor.getPair(l);

                            const auto &_q2 = get_q(triple2);
                            auto &F2 = get_dpdt(triple2);
                            Point2D q2 = _q2;

                            if (i == spaceHash.n - 1) q2.x += SPACE_L;
                            if (j == spaceHash.n - 1) q2.y += SPACE_L;

                            const Point2D F = mdUdr(q1, q2);

                            F1 = F1 + F;
                            F2 = F2 - F;
                        }
                    }
                }
            }
        }
    }

    #else

    // Interaction between molecules within the same container
    {
        assert(v_q.size() == v_p.size() && v_q.size() == v_dpdt.size());
        auto N = v_q.size();

        for (size_t k = 0; k < N-1; ++k) {
            const auto &q1 = v_q[k];
            auto       &F1 = v_dpdt[k];

            for (size_t l = k + 1; l < N; ++l) {
                const auto &q2 = v_q[l];
                auto       &F2 = v_dpdt[l];

                const Point2D F = mdUdr(q1, q2);

                F1 = F1 + F;
                F2 = F2 - F;
            }
        }
    }

    #endif
}


