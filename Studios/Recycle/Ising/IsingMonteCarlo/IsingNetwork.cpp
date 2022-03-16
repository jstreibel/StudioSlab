//
// Created by joao on 15/05/2021.
//

#include "IsingNetwork.h"

#include <iostream>
#include <iomanip>


IsingNetwork::IsingNetwork(int L) : L(L), N(L*L) {
    if(N > MAX_BITSET_SIZE) throw "Bitset too small. Change value of MAX_BITSET_SIZE to accomodate your simulation.";
}

void IsingNetwork::ImprimeCabecalho() const {
    std::cout << "#      1             2              3               4               5              6              7               8              9             10             11\n";
    std::cout << "#      T             Z             <e>             <m>             cv             chi             f             <e^2>          <m^2>          <e>^2         <|m|>^2\n";
}

void IsingNetwork::print(bool datFileSafe) const {
    const bool BIG = false;
    for (int i = 0; i < L; i++) {
        if(datFileSafe) std::cout << "#  ";
        for (int j = 0; j < L; j++) {
            const int si = s(i, j);


            std::string out;
            if(BIG) out = si == 1 ? "\033[41m  \033[0m  " : "\033[42m  \033[0m  ";
            //else  out = si == 1 ? "\033[41m  \033[0m" : "\033[42m  \033[0m"; // vermelho e verde
            //else  out = si == 1 ? "\033[41m  \033[0m" : "\033[44m  \033[0m"; // vermelho e azul
            //else  out = si == 1 ? "\033[43m  \033[0m" : "\033[44m  \033[0m"; // amarelo e azul
            else  out = si == 1 ? "\033[47m  \033[0m" : "\033[100m  \033[0m"; // amarelo e azul

            std::cout << std::setw(3) << out;
        }

        if(BIG) std::cout << "\n#\n";
        else std::cout << "  " << i << "\n\r";

    }
}

int IsingNetwork::ijtok_periodic(int i, int j) const {
    // TODO: while? Deve ter um jeito mais eficiente de fazer isso.....

    while(i<0) i+=L;
    while(j<0) j+=L;

    i = i%L;
    j = j%L;
    return j + i*L; // cond. periodicas de contorno
}

int IsingNetwork::ijtok_abs(int i, int j) const {
    if(i<0 || i>=L || j<0 || j>=L) throw "tidak bagus";

    return j + i*L;
}

int IsingNetwork::s(int i, int j) const {
    //const int k = ((j+L-1)%L) + ((i+L-1)%L) * L; // cond. periodicas de contorno
    const int k = ijtok_periodic(i, j);
    return s(k);
}

int IsingNetwork::s(int k) const {
    return  (S[k]<<1)-1;          // multiplica por dois e subtrai 1
}

double IsingNetwork::E(double h) const {
    int soma=0;
    for(int i=0; i<L; i++)
        for (int j = 0; j < L; j++)
            soma -= s(i,j) * ( s(i+1,j) + s(i,j+1) + h);
    return soma;
}

double IsingNetwork::M() const {
    // obs m_i=s_i

    int mag=0;
    for (int i = 0; i < L; i++)
        for (int j = 0; j < L; j++)
            mag += s(i,j);

    return mag;
}

bool IsingNetwork::areNeighbors(int i1, int j1, int i2, int j2) const {
    // It is expected that the two sites are not the same: !(i1==i2 && j1==j2).

    if(i1==i2){ if(j1==j2-1 || j1==j2+1) return true; }
    else if(j1==j2){ if(i1==i2-1 || i1==i2+1) return true; }

    return false;
}

double IsingNetwork::ssfDeltaE(int i, int j, double h) const {

    return 2*s(i,j)*(s(i-1,j)+s(i+1,j)+s(i,j-1)+s(i,j+1) + h);
}

double IsingNetwork::tseDeltaE(int s1, int s2, double h) const{
    const auto i1=(s1-s1%L)/L,
               j1=s1%L,
               i2=(s2-s2%L)/L,
               j2=s2%L;

    auto dE1 = 2 * s(i1,j1) * (s(i1+1,j1) +  s(i1-1,j1) + s(i1,j1-1) + s(i1,j1+1) + h);
    auto dE2 = 2 * s(i2,j2) * (s(i2+1,j2) +  s(i2-1,j2) + s(i2,j2-1) + s(i2,j2+1) + h);

    if(areNeighbors(i1, j1, i2, j2)){
        dE1 -= s(i2,j2);
        dE2 -= s(i1,j1);
    }

    return (dE1+dE2);
}

void IsingNetwork::flip(int k) {
    S.flip(k);
}

void IsingNetwork::flip(int i, int j) {
    S.flip(ijtok_abs(i, j));
}

void IsingNetwork::operator=(const StateType S) { this->S = S; }

void IsingNetwork::operator=(const unsigned long long S) { this->S = S; }

std::string IsingNetwork::NStates() {
    if(N>32) return std::string("biiiiig");

    return std::to_string(1<<N);;
}

void IsingNetwork::set(int k, bool value) {
    S[k] = value;
}

int IsingNetwork::e(int i, int j) const {
    return s(i,j)*(s(i-1,j)+s(i+1,j)+s(i,j-1)+s(i,j+1));
}





