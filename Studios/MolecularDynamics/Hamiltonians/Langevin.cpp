#include "Langevin.h"

#include "Hamiltonians/SoftDisk/SoftDiskParams.h"

#define FRANDOM (random()/(RAND_MAX+1.0))

namespace MolecularDynamics {

    Langevin::Langevin(const NumericParams &p, Real T) : NewtonMechanics(p), T(T), dt(p.getdt()) {    }

    Point2D Langevin::xi() {
        const Real z = FRANDOM, theta = 2.0 * M_PI * FRANDOM;
        const Real r = sqrt(-2.0 * log(1.0 - z));

        return {r * cos(theta), r * sin(theta)};
    }

    Point2D Langevin::F_nh(Real) {
        fix alpha = sqrt(2 * T / dt);
        return alpha * xi();
    }

    void Langevin::setTemperature(Real temperature) {
        T = temperature;
    }


    // Reference (equilibrium) implementation:
    /*
    void Langevin::operator()(const MoleculeContainer &m, MoleculeContainer &dmdt, const Real t ) {
        const Real alpha = sqrt(2 * T / dt);

        for (auto &dm: dmdt) {
            dm.m_q = alpha * xi();
            dm.m_p = {0., 0.};
        }

        // INTERACTION BETWEEN MOLECULES
        for (size_t i = 0; i < m.size() - 1; i++) {
            OUT F1 = dmdt[i].m_q;
            IN q1 = m[i].m_q, &p1 = m[i].m_p;

            for (size_t j = i + 1; j < m.size(); j++) {
                OUT F2 = dmdt[j].m_q;
                IN q2 = m[j].m_q;

                let F = dUdr(q1, q2);

                F1 += F;
                F2 -= F;
            }
        }
    }
    */
}