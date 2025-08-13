//
// Created by joao on 25/06/2021.
//

#ifndef MARYLOUSTRINGS_INTEGRATOR_H
#define MARYLOUSTRINGS_INTEGRATOR_H


#include <vector>
#include "Utils/Types.h"

#include "AppBase.h"
#include "Core/Controller/Nuklear/NuklearSFML.h"
#include "Graphics/SFML/Graph.h"

#include <boost/numeric/odeint.hpp>

typedef Vector<Real> Funcs;


struct Output {
    virtual void operator()( const Funcs &x , double t ) = 0;
};

struct OutputToVector : public Output
{
    Vector< Funcs > m_states;
    Vector< double > m_times;

    OutputToVector() = default;

    void clear();

    void operator()( const Funcs &x , double t );
};

struct OutputHelper {
    Output *output = nullptr;
    explicit OutputHelper(Output *output);
    virtual void operator()( const Funcs &x , double t );;
};


struct StringsOfMarylou {

    const DevFloat beta, alpha_phi;

    inline void RHS(const Funcs &V, Funcs &dVdx, const double x) const;

    void operator()(const Funcs &V, Funcs &dVdx, const DevFloat x);
};

class MaryApp : public AppBase {
    OutputToVector output;

    float w = 300.f;
    int rectLeft = 1.1 * w + 4 * 20;

    Vector<Lost::Graph> graphs;
    float start_x = 0.0001, end_x = 5, dx = 1.e-5 ;
    float f0 = 1.09868f, dgdx0 = 1.31902f, dhdx0 = 0.000119189;

    //Vector<unsigned> colorInts({0xbeef9eff, 0xa6c36fff,0x828c51ff,0x335145ff,0x1e352fff,0x7d5ba6ff});
    //Vector<unsigned> colorInts({0xe27396ff,0xea9ab2ff,0xe0e2dbff,0xd2d4c8ff,0xd3efbdff,0x447604ff});
    const Vector<unsigned> ci{ 0x54478cff, 0x2c699aff, 0x048ba8ff, 0x0db39eff, 0x16db93ff, 0x83e377ff, 0xb9e769ff, 0xefea5aff, 0xf1c453ff, 0xf29e4cff };
    const Vector<unsigned> colorInts{ ci[0], ci[3], ci[6], ci[1], ci[4], ci[7] };

    Vector<sf::Color> colors = Vector<sf::Color>(6);

    void updateGraph();
public:
    MaryApp(sf::RenderWindow &window);

    int run() override;

    void setupGUI(nk_context *ctx);

    void draw(sf::RenderWindow &window);
};


#endif //MARYLOUSTRINGS_INTEGRATOR_H
