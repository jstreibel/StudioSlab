//
// Created by joao on 15/05/2021.
//

#include "IsingNetwork5X5Max.h"
#include <string>

IsingNetwork_5x5max::IsingNetwork_5x5max(int L, IsingState S0) : L(L), N(L * L), S(S0) {
    if(L*L>32) throw "Too big for bits.";
}

void IsingNetwork_5x5max::ImprimeCabecalho() const {
    std::cout << "#      1             2              3               4               5              6              7               8              9             10             11\n";
    std::cout << "#      T             Z             <e>             <m>             cv             chi             f             <e^2>          <m^2>          <e>^2         <|m|>^2\n";
}

void IsingNetwork_5x5max::print(bool datFileSafe) const {
    const bool BIG = true;
    for (int i = 0; i < L; i++) {
        if(datFileSafe) std::cout << "#  ";
        for (int j = 0; j < L; j++) {
            const int si = s(i, j);

            std::string out;
            if(BIG) out = si == 1 ? "\033[41m  \033[0m  " : "\033[42m  \033[0m  ";
            else  out = si == 1 ? "\033[41m  \033[0m" : "\033[42m  \033[0m";

            std::cout << std::setw(3) << out;
        }
        std::cout << "\n#\n";
    }
}

int IsingNetwork_5x5max::ijtok_periodic(int i, int j) const {
    while(i<0) i+=L;
    while(j<0) j+=L;

    i = i%L;
    j = j%L;
    return j + i*L; // cond. periodicas de contorno
}

int IsingNetwork_5x5max::ijtok_abs(int i, int j) const {
    if(i<0 || i>=L || j<0 || j>=L) throw "tidak bagus";

    return j + i*L;
}

int IsingNetwork_5x5max::s(int i, int j) const {
    //const int k = ((j+L-1)%L) + ((i+L-1)%L) * L; // cond. periodicas de contorno
    const int k = ijtok_periodic(i, j);
    return s(k);
}

int IsingNetwork_5x5max::s(int k) const {
    int _s = int(S & (1 << k)); _s = _s >> k; // pega o bit (0 ou 1)
    return  ((_s<<1)-1);          // multiplica por dois e subtrai 1
}

double IsingNetwork_5x5max::E() const {
    int soma=0;
    for(int i=0; i<L; i++)
        for (int j = 0; j < L; j++)
            soma -= s(i,j) * ( s(i+1,j) + s(i,j+1) );
    return soma;
}

double IsingNetwork_5x5max::M() const {
    // obs m_i=s_i

    int mag=0;
    for (int i = 0; i < L; i++)
        for (int j = 0; j < L; j++)
            mag += s(i,j);

    return mag;
}

double IsingNetwork_5x5max::ssfDeltaE(int i, int j) const {
    return 2*s(i,j)*(s(i-1,j)+s(i+1,j)+s(i,j-1)+s(i,j+1));
}

void IsingNetwork_5x5max::flip(int i, int j) {
    const int k = ijtok_abs(i, j);
    IsingState S_ = S;
    S ^= 1UL << k;

    //if(S==S_) std::cout << "\nno sf @ " << i << j << "\n";
    //else      std::cout << "\nssf @ "   << i << j << "\n";

}

void IsingNetwork_5x5max::operator=(const IsingState S) { this->S = S; }

void IsingNetwork_5x5max::operator=(const int S) { this->S = S; }

bool IsingNetwork_5x5max::operator<(const int N) const { return S<N; }

IsingNetwork_5x5max &IsingNetwork_5x5max::operator++() { ++S; return *this; }

uint64_t IsingNetwork_5x5max::NStates() {
    if(N>64) throw "BIIIIG";

    return 1<<N;
}

