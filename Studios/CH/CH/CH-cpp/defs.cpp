#include "defs.h"

/*
Point2D operator + (const Point2D a, const Point2D b){ return {a.x+b.x, a.y+b.y}; }
Point2D operator - (const Point2D a, const Point2D b){ return {a.x-b.x, a.y-b.y}; }
Point2D operator * (const double a, const Point2D p){ return {p.x*a, p.y*a}; }

Molecul operator*(const double &a, const Molecul &m){ return {a*m.q, a*m.p}; }
Molecul operator+(const Molecul &a, const Molecul &b){ return {a.q+b.q, a.p+b.p}; }


void ConsoleOutput::operator()( const MoleculContainer &molecules , double t) const
{
    const Point2D &q1 = molecules[0].q;
    const Point2D &q2 = molecules[1].q;
    std::cout << q1.x << " " << q1.y << "\n";
    std::cout << q2.x << " " << q2.y;
    std::cout << std::endl << std::endl;
}
*/

void resizeMat(MatDoub &mat, size_t H, size_t W){
    MatDoub tempmat;

    size_t ni = mat.size();
    size_t nj = ni == 0 ? 0 : mat[0].size();

    for (size_t i=0; i<ni; i++)
        for (size_t j=0; j<nj; j++)
            tempmat[i][j] = mat[i][j];

    mat.resize(H);

    for (size_t i=0; i < H; ++i)
        mat[i].resize(W);

    for(size_t i=0; i<ni; i++)
        for(size_t j=0; j<nj; j++)
            mat[i][j] = tempmat[i][j];
}
