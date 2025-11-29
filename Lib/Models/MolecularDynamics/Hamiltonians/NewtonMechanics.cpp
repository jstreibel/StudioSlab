//
// Created by joao on 05/08/2021.
//

#include <sstream>

#include "NewtonMechanics.h"
#include "Models/MolecularDynamics/Hamiltonians/Hash/HashParams.h"
#include "Particle.h"
#include "Core/Tools/Log.h"


#define USE_NEW_EXPERIMENTAL_IMPLEMENTATION false
#define DISSIPATION_FACTOR 1e-3

namespace Slab::Models::MolecularDynamics {

    NewtonMechanics::NewtonMechanics(TPointer<Config> config)
    : dissipation(DISSIPATION_FACTOR)
    , spaceHash(HASH_SUBDIVS, config->GetL())
    , numeric_config(config)
    , flippedSides(new bool[config->getN()]) {
        if(USE_NEW_EXPERIMENTAL_IMPLEMENTATION) {
            if (spaceHash.totalLength() != config->GetL()) {
                Core::Log::ErrorFatal() << "NewtonMechanics Hashspace inconsistency. Hashspace total width is "
                             << spaceHash.totalLength()
                             << " while sim space is " << config->GetL();

                throw Exception("Hashspace inconsistency");

            } else if (spaceHash.l <= CUTOFF_RADIUS) {
                Core::Log::ErrorFatal() << "NewtonMechanics Hashspace inconsistency. Hashspace box length is " << spaceHash.l
                             << " while molecule cutoff radius is " << CUTOFF_RADIUS << Core::Log::Flush;

                throw Exception("Hashspace inconsistency");
            }
        }

        for(auto i=0; i<config->getN(); ++i)
            flippedSides[i] = false;
    }

    NewtonMechanics::~NewtonMechanics() = default;


    void NewtonMechanics::applyBoundaryConditions(Graphics::PointContainer & v_q) {
        fix L = numeric_config->GetL();

        const DevFloat hw = L * .5;
        int i = 0;
        for (auto &q : v_q) {
            if (q.x < -hw || q.x >= hw) {
                q.x -= L* floor(q.x/L + .5);
                flippedSides[i] = true;
            }

            if (q.y < -hw || q.y >= hw) {
                q.y -= L* floor(q.y/L + .5);
                flippedSides[i] = true;
            }

            i++;
        }
    }

    void NewtonMechanics::applyBoundaryConditions(MoleculeContainer &v_m) {
        fix L = numeric_config->GetL();

        fix hw = L * .5;
        int i = 0;
        for (auto &m : v_m) {
            auto &q = m.m_q;

            if (q.x < -hw || q.x >= hw) {
                q.x -= L* floor(q.x/L + .5);
                flippedSides[i] = true;
            }

            if (q.y < -hw || q.y >= hw) {
                q.y -= L* floor(q.y/L + .5);
                flippedSides[i] = true;
            }

            i++;
        }
    }


    DevFloat NewtonMechanics::computeEnergy(const Graphics::PointContainer &v_q, Graphics::PointContainer &v_p) {
        DevFloat K = .0;
        for (auto &p : v_p)
            K += p.LengthSqr();
        K *= .5;

        const size_t N = v_q.size();
        DevFloat _U = 0.0;
        for (size_t i = 0; i < N - 1; i++) {
            const Graphics::FPoint2D &q1 = v_q[i];
            for (size_t j = i; j < N; j++) {
                const Graphics::FPoint2D &q2 = v_q[j];

                _U += U(q1, q2);
            }
        }

        return K + _U;
    }

    void
    NewtonMechanics::operator()(const Graphics::PointContainer &v_q, const Graphics::PointContainer &v_p, Graphics::PointContainer &v_dpdt,
                                double t) {
        applyBoundaryConditions(const_cast<Graphics::PointContainer &>(v_q));

        // Single molecule potential + non-homogenous
        for (int i = 0; i < v_q.size(); i++) {
            // IN q = v_q[i];
            IN p = v_p[i];
            OUT F = v_dpdt[i];

            F = -dissipation*p + F_nh(t);
        }

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

        #elif USE_NEW_EXPERIMENTAL_IMPLEMENTATION && false // NOT SO EXPERIMENTAL

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

        {
            assert(v_q.size() == v_p.size() && v_q.size() == v_dpdt.size());
            auto N = v_q.size();

            for (size_t k = 0; k < N-1; ++k) {
                const auto &q1 = v_q[k];
                auto       &F1 = v_dpdt[k];

                for (size_t l = k + 1; l < N; ++l) {
                    const auto &q2 = v_q[l];
                    auto       &F2 = v_dpdt[l];

                    const Graphics::FPoint2D F = dUdr(q1, q2);

                    F1 += F;
                    F2 -= F;
                }
            }
        }

        #endif
    }

    void NewtonMechanics::operator()(const MoleculeContainer &v_m, MoleculeContainer &v_dmdt, const DevFloat) {
        const auto N = v_m.size();

        assert(N == v_dmdt.size());

        applyBoundaryConditions(const_cast<MoleculeContainer &>(v_m));

        for (auto &dm: v_dmdt)
            dm.m_p = {0., 0.};

        for(auto i=0; i<N-1; ++i) {
            IN q1 = v_m[i].m_q;
            OUT F1 = v_dmdt[i].m_p;

            for(auto j=i+1; j<N; ++j) {
                IN q2 = v_m[j].m_q;
                OUT F2 = v_dmdt[j].m_p;

                let F = dUdr(q1, q2);

                F1 += F;
                F2 -= F;
            }
        }

    }

    auto NewtonMechanics::getFlippedSidesMap() const -> const bool * {
        return flippedSides;
    }

    auto NewtonMechanics::setDissipation(DevFloat k) -> void {
        dissipation = k;
    }

}
