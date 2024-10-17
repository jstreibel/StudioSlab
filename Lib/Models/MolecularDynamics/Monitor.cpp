//
// Created by joao on 9/08/23.
//

#include "Monitor.h"

#include "Core/Backend/SFML/SFMLBackend.h"
#include "Models/MolecularDynamics/Hamiltonians/Particle.h"

#include "Hamiltonians/Lennard-Jones/LennardJonesParams.h"
#include "Hamiltonians/Lennard-Jones/LennardJones.h"

#include "Hamiltonians/SoftDisk/SoftDisk.h"

#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"

namespace MolecularDynamics {

#define SHOW_DOT false
#define SHOW_RADIUS true

#define SFML_Backend dynamic_cast<Core::SFMLBackend&>(Core::BackendManager::GetGUIBackend())

    Monitor::Monitor(Pointer<Config> config, Model model)
    : Socket("Particle dynamics monitor", 10)
    , Window(0, 0, 100, 100, HasMainMenu)
    , renderWindow(SFML_Backend.getRenderWindow())
    , molShapes(2*N)
    , molShape(CUTOFF_RADIUS, 36)
    , molTexture()
    , N(config->getN()), L(float(config->getL()))
    {
        sf::Color skyBlue(135, 206, 235, 255);
        sf::Color someRed(235, 206, 135, 255);

        auto negColor = skyBlue;
        auto posColor = someRed;

        {
            sf::Image molImg;
            const int texDim = 256;

            Pair<double, double> nearestZero = {1.e3, .0},
                    smallest    = {.0,   .0},
                    largest     = {.0,   .0};

            molImg.create(texDim, texDim, sf::Color(255, 0, 255, 0));
            for (auto i = 0; i < texDim; ++i) {
                for (auto j = 0; j < texDim; ++j) {
                    const double x = 2.*(i/double(texDim) - .5);
                    const double y = 2.*(j/double(texDim) - .5);
                    const double r = sqrt(x*x + y*y)*CUTOFF_RADIUS;

                    if(r>=0.98*CUTOFF_RADIUS && SHOW_RADIUS && false)
                        molImg.setPixel(i, j, sf::Color(0, 255, 0, 255));
                    else if(r < 0.5*1.12*σ && SHOW_DOT)
                        molImg.setPixel(i, j, sf::Color(255, 255, 255, 255));
                    else if(!SHOW_DOT){
                        Real U;
                        Real factor;

                        if     (model == Model::LennardJones){ U = LennardJones::U(r); factor = U/ε; }
                        else if(model == Model::SoftDisk)    { U = SoftDisk::    U(r); factor = U; }

                        auto alpha = 255.0;
                        auto color = posColor;

                        if(U<0){
                            color = negColor;
                            alpha *= factor;
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

            Core::Log::Info("ParticleDynamics::Monitor: ")
                      << "\n\t\t\t\t\tNearest zero " << nearestZero.first << " @ r=" << nearestZero.second
                      << "\n\t\t\t\t\tSmallest     " << smallest.first << " @ r=" << smallest.second
                      << "\n\t\t\t\t\tLargest      " << largest.first << " @ r=" << largest.second;
            molTexture.loadFromImage(molImg);
        }

        molShape.setTexture(&molTexture);
        if(SHOW_RADIUS) {
            molShape.setOutlineThickness(0.1);
            auto color = skyBlue;
            color.a = 50;
            molShape.setOutlineColor(color);
        }
        molShape.setOrigin(CUTOFF_RADIUS, CUTOFF_RADIUS);
    }

    void Monitor::handleOutput(const Math::OutputPacket &packet) {    }

    void Monitor::draw() {
        // Window::draw();

        auto state = lastPacket.GetNakedStateData<State>();
        auto v_q = state->first();
        auto v_p = state->second();

        const auto O = .0f;

        const auto L_bitMore = (float)L * 1.2f;
        sf::Color bg(0 , 0, 0);
        sf::Color skyBlue(135, 206, 235, 32);

        const float ratio = (float)renderWindow.getSize().x / (float)renderWindow.getSize().y;
        renderWindow.setView(sf::View(sf::Vector2f(.0, .0), sf::Vector2f(L_bitMore*ratio, -L_bitMore)));
        renderWindow.clear(bg);

        sf::Shape &moleculeShape = molShape;

        // Hash grid
        {
            sf::Color umPreto(32, 32, 32);
            sf::Color cor = umPreto;
            auto l = L / HASH_SUBDIVS;
            auto base = sf::Vector2f{-.5f*L, -.5f*L};
            Vector<sf::Vertex> subdivs;

            for(auto i=1; i<HASH_SUBDIVS; ++i){
                auto il = float(i)*l;
                subdivs.emplace_back(base + sf::Vector2f{0, il}, cor);
                subdivs.emplace_back(base + sf::Vector2f{L, il}, cor);

                subdivs.emplace_back(base + sf::Vector2f{il, 0}, cor);
                subdivs.emplace_back(base + sf::Vector2f{il, L}, cor);
            }

            renderWindow.draw(&subdivs[0], subdivs.size(), sf::Lines);
        }


        // Particles
        {
            sf::Vector2f offsets[9] = {{-L, -L},
                                       { O, -L},
                                       { L, -L},
                                       {-L,  O},
                                       { O,  O},
                                       { L,  O},
                                       {-L,  L},
                                       { O,  L},
                                       { L,  L}};
            for (auto &q : v_q) {
                int c = 0;
                for (auto &o : offsets) {
                    moleculeShape.setPosition(sf::Vector2f((float)q.x, (float)q.y) + o);
                    if (c == 4)
                        moleculeShape.setFillColor(sf::Color(255, 255, 255, 100));
                    else
                        moleculeShape.setFillColor(sf::Color(255, 255, 255, 65));

                    c++;
                    renderWindow.draw(moleculeShape);
                }
            }
            renderWindow.draw(&molShapes[0], N, sf::Lines);
        }


        // Boundaries
        {
            sf::Color rosaChoqueDaNina(255, 20, 147);
            sf::Color cor = rosaChoqueDaNina;
            sf::Vertex box[] = {
                    sf::Vertex(sf::Vector2f(-.5f * L,  .5f * L), cor),
                    sf::Vertex(sf::Vector2f( .5f * L,  .5f * L), cor),
                    sf::Vertex(sf::Vector2f( .5f * L, -.5f * L), cor),
                    sf::Vertex(sf::Vector2f(-.5f * L, -.5f * L), cor),
                    sf::Vertex(sf::Vector2f(-.5f * L,  .5f * L), cor)
            };
            renderWindow.draw(box, 5, sf::LineStrip);
        }


        // Big circle indicating the interaction cutoff radius
        if(0)
        {
            sf::CircleShape molRadius(CUTOFF_RADIUS);
            molRadius.setPosition(-.5f * L - 3 * CUTOFF_RADIUS, .5f * L - 2 * CUTOFF_RADIUS);
            renderWindow.draw(molRadius);

            // molShape.setPosition( -.5f * L - 3 * CUTOFF_RADIUS, .5f * L - 5 * CUTOFF_RADIUS);
            // renderWindow.draw(molShape);
        }
    }



} // MolecularDynamics