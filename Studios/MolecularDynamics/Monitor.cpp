//
// Created by joao on 9/08/23.
//

#include "Monitor.h"

#include "Base/Backend/SFML-Nuklear/SFML-Nuklear-Backend.h"
#include "Particle.h"
#include "Hamiltonians/Lennard-Jones/LennardJonesParams.h"
#include "Hamiltonians/Lennard-Jones/LennardJones.h"

namespace MolecularDynamics {

#define SHOW_DOT false

    Monitor::Monitor(const NumericParams &params)
    : Numerics::OutputSystem::Socket(params, "Particle dynamics monitor", 10)
    , Window()
    , renderWindow(Backend::GetInstanceSuper<SFMLNuklearBackend>().getRenderWindow()) {
        sf::Color skyBlue(135, 206, 235, 32);
        sf::Color someRed(235, 206, 135, 32);

        auto negColor = skyBlue;
        auto posColor = someRed;

        {
            sf::Image molImg;
            const int texDim = 512;

            std::pair<double, double> nearestZero = {1.e3, .0},
                    smallest    = {.0,   .0},
                    largest     = {.0,   .0};

            molImg.create(texDim, texDim, sf::Color(255, 0, 255, 0));
            for (auto i = 0; i < texDim; ++i) {
                for (auto j = 0; j < texDim; ++j) {
                    const double x = 2.*(i/double(texDim) - .5);
                    const double y = 2.*(j/double(texDim) - .5);
                    const double r = sqrt(x*x + y*y)*CUTOFF_RADIUS;

                    if(r>=CUTOFF_RADIUS)
                        molImg.setPixel(i, j, sf::Color(0, 255, 0, 0));
                    else if(r < 0.5*1.12*σ && SHOW_DOT)
                        molImg.setPixel(i, j, sf::Color(255, 255, 255, 255));
                    else if(!SHOW_DOT){
                        const auto U = LennardJones::U(r);

                        auto alpha = 255.0;
                        auto color = posColor;
                        auto factor = U/ε;

                        // if(i==texDim/2 && j>=texDim/2) std::cout << "\nU(" << r << ")/eps=" << factor;

                        if(U<0){
                            color = negColor;
                            //factor = log10(1-9*factor);
                            alpha *= -factor;
                        }  else {
                            alpha *= (factor>1?1.0:factor);
                        }

                        color.a = alpha;

                        molImg.setPixel(i, j, color);

                        if(fabs(U)<fabs(nearestZero.first)) nearestZero={U, r};
                        if(U<smallest.first)                smallest   ={U, r};
                        if(U>largest.first)                 largest    ={U, r};
                    }
                }
            }

            std::cout << "\nNearest zero: " << nearestZero.first << " @ r=" << nearestZero.second
                      << "\nSmallest: " << smallest.first << " @ r=" << smallest.second
                      << "\nLargest: " << largest.first << " @ r=" << largest.second << std::endl;
            molTexture.loadFromImage(molImg);
        }

        molShape.setTexture(&molTexture);
        //molShape.setOutlineThickness(0.05);
        //molShape.setOutlineColor(skyBlue);
        molShape.setOrigin(CUTOFF_RADIUS, CUTOFF_RADIUS);

        /*
        for(sf::Vertex *track : moleculesHistory) {
            for (int i = 0; i < MOLS_HISTORY_SIZE; i++) {
                const float factor = (float(i + 1) / MOLS_HISTORY_SIZE);
                const auto val = sf::Uint8(128 * factor);
                track[i] = sf::Vertex({0, 0},
                                      {255, 255, 255, val});
            }
        }
         */
    }

    void Monitor::_out(const OutputPacket &packet) {    }

    void Monitor::draw() {
        // Window::draw();

        auto state = lastData.getEqStateData<State>();
        auto v_q = state->first;
        auto v_p = state->second;

        const double l = SPACE_L * 1.2;
        sf::Color bg(0 , 0, 0);
        sf::Color skyBlue(135, 206, 235, 32);

        const float ratio = renderWindow.getSize().x / float(renderWindow.getSize().y);
        renderWindow.setView(sf::View(sf::Vector2f(.0, .0), sf::Vector2f(l*ratio, -l)));
        renderWindow.clear(bg);

        sf::Shape &moleculeShape = molShape;
        //float radius = 0.06;
        //sf::CircleShape moleculeShape = sf::CircleShape(radius, 8);
        //moleculeShape.setOrigin(radius, radius);
        //moleculeShape.setFillColor(sf::Color(255, 255, 255, 100));

        const float L = SPACE_L, O = 0;


        // Hash grid
        {
            sf::Color umPreto(32, 32, 32);
            sf::Color cor = umPreto;
            auto l = L / HASH_SUBDIVS;
            auto base = sf::Vector2f{-.5f*L, -.5f*L};
            std::vector<sf::Vertex> subdivs;

            for(auto i=1; i<HASH_SUBDIVS; ++i){
                subdivs.emplace_back(sf::Vertex(base + sf::Vector2f{0, i*l}, cor));
                subdivs.emplace_back(sf::Vertex(base + sf::Vector2f{L, i*l}, cor));

                subdivs.emplace_back(sf::Vertex(base + sf::Vector2f{i*l, 0}, cor));
                subdivs.emplace_back(sf::Vertex(base + sf::Vector2f{i*l, L}, cor));
            }

            renderWindow.draw(&subdivs[0], subdivs.size(), sf::Lines);
        }


        // Particles
        {
            sf::Vector2f offsets[9] = {{-L, -L},
                                       {O,  -L},
                                       {L,  -L},
                                       {-L, O},
                                       {O,  O},
                                       {L,  O},
                                       {-L, L},
                                       {O,  L},
                                       {L,  L}};
            for (auto &q : v_q) {
                int c = 0;
                for (auto &o : offsets) {
                    moleculeShape.setPosition(sf::Vector2f(q.x, q.y) + o);
                    if (c == 4)
                        moleculeShape.setFillColor(sf::Color(255, 255, 255, 100));
                    else
                        moleculeShape.setFillColor(sf::Color(255, 255, 255, 65));

                    c++;
                    renderWindow.draw(moleculeShape);
                }
            }
            // renderWindow.draw(&molShapes[0], N_MOLS, sf::Lines);
        }


        // Boundaries
        {
            sf::Color rosaChoqueDaNina(255, 20, 147);
            sf::Color cor = rosaChoqueDaNina;
            sf::Vertex box[] = {
                    sf::Vertex(sf::Vector2f(-.5 * SPACE_L, .5 * SPACE_L), cor),
                    sf::Vertex(sf::Vector2f(.5 * SPACE_L, .5 * SPACE_L), cor),
                    sf::Vertex(sf::Vector2f(.5 * SPACE_L, -.5 * SPACE_L), cor),
                    sf::Vertex(sf::Vector2f(-.5 * SPACE_L, -.5 * SPACE_L), cor),
                    sf::Vertex(sf::Vector2f(-.5 * SPACE_L, .5 * SPACE_L), cor)
            };
            renderWindow.draw(box, 5, sf::LineStrip);
        }


        // Big circle indicating the interaction cutoff radius
        {
            sf::CircleShape molRadius(CUTOFF_RADIUS);
            molRadius.setPosition(-.5 * L - 3 * CUTOFF_RADIUS, .5 * L - 2 * CUTOFF_RADIUS);
            renderWindow.draw(molRadius);
        }
    }



} // MolecularDynamics