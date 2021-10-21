#ifndef LANGEVIN_H
#define LANGEVIN_H

#include "defs.h"

/*
class Langevin
{
public:
    Langevin(double T, double dt);

    void apply_bc(MoleculContainer &mols);
    double computeEnergy(const std::vector<Point2D> &v_q, std::vector<Point2D> &v_p);

    void operator() ( const MoleculContainer &m, MoleculContainer &dmdt, const double t);

private:
    double _U(const Point2D &q1, const Point2D &q2);
    Point2D _mdUdr (const Point2D &q1, const Point2D &q2);
    Point2D _xi();

private:
    double T, dt;
};
*/

#endif // LANGEVIN_H
