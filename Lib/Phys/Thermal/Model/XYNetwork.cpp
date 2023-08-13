//
// Created by joao on 15/05/2021.
//

#include "XYNetwork.h"

#include <iostream>
#include <iomanip>

XYNetwork::XYNetwork(int L) : L(L), N(L*L), ThetaField(L * L) {

}

int XYNetwork::ij_to_k_periodic(int i, int j) const {
    // TODO: while? Deve ter um jeito mais eficiente de fazer isso.....

    while(i<0) i+=L;
    while(j<0) j+=L;

    i = i%L;
    j = j%L;
    return j + i*L; // cond. periodicas de contorno
}

int XYNetwork::ij_to_k_abs(int i, int j) const {
    if(i<0 || i>=L || j<0 || j>=L) throw "tidak bagus";

    return j + i*L;
}

Real XYNetwork::theta(int i, int j) const {
    const int k = ij_to_k_periodic(i, j);
    return theta(k);
}

Real XYNetwork::theta(int k) const {
    return  ThetaField[k];
}

Real XYNetwork::E(Real h) const {
    Real soma=0;
    for(int i=0; i<L; i++)
        for (int j = 0; j < L; j++) {
            float th = theta(i, j);
            float th_east = theta(i + 1, j);
            float th_south = theta(i, j + 1);

            soma -= cos(th-th_east) + cos(th-th_south);
        }

    return soma;
}

Real XYNetwork::M() const {
    // obs m_i=s_i

    Real magx=0;
    Real magy=0;
    for (int i = 0; i < L; i++)
        for (int j = 0; j < L; j++) {
            auto th = theta(i, j);
            magx += cos(th);
            magy += sin(th);
        }

    return sqrt(magx*magx + magy*magy);
}

bool XYNetwork::areNeighbors(int i1, int j1, int i2, int j2) const {
    // It is expected that the two sites are not the same: !(i1==i2 && j1==j2).

    if(i1==i2){ if(j1==j2-1 || j1==j2+1) return true; }
    else if(j1==j2){ if(i1==i2-1 || i1==i2+1) return true; }

    return false;
}

Real XYNetwork::ssrDeltaE(int i, int j, Real h, Real delta) const {
    auto thC = theta(i, j);

    auto thN = theta(i, j - 1),
         thS = theta(i, j + 1),
         thE = theta(i + 1, j),
         thW = theta(i - 1, j);

    #define deltaE_viz(th_viz) (cos(thC-(th_viz)) - cos(thC-(th_viz) + delta))

    auto vizN_deltaE = deltaE_viz(thN),
         vizS_deltaE = deltaE_viz(thS),
         vizE_deltaE = deltaE_viz(thE),
         vizW_deltaE = deltaE_viz(thW);

    return vizN_deltaE + vizS_deltaE + vizE_deltaE + vizW_deltaE;
}

Real XYNetwork::ssorrDeltaE(int i, int j, Real h, Real delta) const {
    throw "Not implemented.";
    return 0;
}

void XYNetwork::overrelax(int i, int j) {
    const auto θ  = theta(i, j),
               θN = theta(i, j+1),
               θS = theta(i, j-1),
               θE = theta(i+1, j),
               θW = theta(i-1, j);

    const auto h_x = cos(θN) + cos(θS) + cos(θE) + cos(θW),
               h_y = sin(θN) + sin(θS) + sin(θE) + sin(θW);

    const auto h2 = h_x*h_x + h_y*h_y;
    //const auto h = sqrt(h2);

    auto S_x = cos(θ),
         S_y = sin(θ);

    const auto h_dot_S = h_x*S_x + h_y*S_y;

    S_x = -S_x + 2.*h_x*h_dot_S/h2;
    S_y = -S_y + 2.*h_y*h_dot_S/h2;

    auto newθ = atan(S_y/S_x);

    set(ij_to_k_abs(i,j), newθ);
}

double XYNetwork::tseDeltaE(int s1, int s2, double h) const{
    throw "Not implemented tseDeltaE";

    const auto i1=(s1-s1%L)/L,
               j1=s1%L,
               i2=(s2-s2%L)/L,
               j2=s2%L;

    auto dE1 = 2 * theta(i1, j1) * (theta(i1 + 1, j1) + theta(i1 - 1, j1) + theta(i1, j1 - 1) + theta(i1, j1 + 1) + h);
    auto dE2 = 2 * theta(i2, j2) * (theta(i2 + 1, j2) + theta(i2 - 1, j2) + theta(i2, j2 - 1) + theta(i2, j2 + 1) + h);

    if(areNeighbors(i1, j1, i2, j2)){
        dE1 -= theta(i2, j2);
        dE2 -= theta(i1, j1);
    }

    return (dE1+dE2);
}

void XYNetwork::rotate(int k, Real angle) {
    ThetaField[k] += angle;
}

void XYNetwork::rotate(int i, int j, Real angle) {
    rotate(ij_to_k_abs(i, j), angle);
}

void XYNetwork::operator=(const StateType S) { this->ThetaField = S; }


void XYNetwork::set(int k, Real value) {
    ThetaField[k] = value;
}

Real XYNetwork::e(int i, int j) const {
    auto thC = theta(i, j);

    auto thN = theta(i, j - 1),
            thS = theta(i, j + 1),
            thE = theta(i + 1, j),
            thW = theta(i - 1, j);

    return -(cos(thC-thN) + cos(thC-thS) + cos(thC-thE) + cos(thC-thW));
}





