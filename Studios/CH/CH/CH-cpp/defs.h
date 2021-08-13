#ifndef DEFS_H
#define DEFS_H

/*
#define EPSILON 1.0
#define SIGMA 1.0 // raio da interacao
#define ALPHA 2.5
#define SIM_AREA 2.00

#include <vector>
#include <iostream>
#include <cmath>

struct Point2D {
    Point2D() {}
    Point2D(double x, double y) : x(x), y(y) {}

    Point2D operator * (const double a) const {
        return Point2D(x*a, y*a);
    }

    Point2D operator - () const { return Point2D(-x,-y); }

    inline double normSqr() const { return x*x + y*y;}
    inline double norm( ) const { return sqrt(normSqr());}

    double x, y;
};
Point2D operator + (const Point2D a, const Point2D b);
Point2D operator - (const Point2D a, const Point2D b);
Point2D operator * (const double a, const Point2D p);

typedef std::vector<Point2D> PointContainer;

struct Molecul {
    Molecul(Point2D q, Point2D p)
        : q(q), p(p) {    }
    Molecul() {}

    Point2D q, p;
};
Molecul operator*(const double &a, const Molecul &m);
Molecul operator+(const Molecul &a, const Molecul &b);

typedef std::vector<Molecul> MoleculContainer;

class ConsoleOutput {
public:
    void operator()( const MoleculContainer &    molecules   , double t) const;
};
*/

/* CAHN-HILLIARD */

#include <limits>
#include <vector>

#include <cstdlib>
#include <cstdio>
#include <math.h>

#ifndef INLINE
#ifdef MSC_VER
#define INLINE __forceinline
#else
#define INLINE inline

#endif
#endif

typedef std::vector<double> VecDoub;
typedef VecDoub VecDoub_O;
typedef const VecDoub VecDoub_I;
typedef VecDoub VecDoub_IO;
typedef std::vector<std::vector<double> > MatDoub;

typedef int Int;
typedef double Doub;

inline double SIGN (const double &a, const double &b) { return ((b) >= 0.0 ? fabs(a) : -fabs(a)); }
inline double MIN (const double &a, const double &b) { return ((a < b) ? a : b); }
inline double MAX (const double &a, const double &b) { return ((a > b) ? a : b); }
inline double sqr (const double &v) { return v*v; }

//void throwEx (const char *message);

extern "C" inline void resizeMat(MatDoub &mat, int H, int W);

//void throwEx (const char *message) { printf(message, NULL); exit(-1); }

void resizeMat(MatDoub &mat, size_t H, size_t W);

#endif // DEFS_H
