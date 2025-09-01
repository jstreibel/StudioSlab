//
// Created by joao on 9/08/23.
//

#include "Monitor.h"

#include "Graphics/SlabGraphics.h"
#include "Graphics/Backend/SFML/SFMLBackend.h"
#include "Models/MolecularDynamics/Hamiltonians/Particle.h"

#include "Hamiltonians/Lennard-Jones/LennardJonesParams.h"
#include "Hamiltonians/Lennard-Jones/LennardJones.h"

#include "Hamiltonians/SoftDisk/SoftDisk.h"

#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"
#include "Models/MolecularDynamics/Hamiltonians/MoleculesState.h"

namespace Slab::Models::MolecularDynamics {

    #define SHOW_DOT true
    #define SHOW_RADIUS true
    #define SHOW_HASH_GRID false

    #define SFML_Backend DynamicPointerCast<Graphics::SFMLBackend>(Slab::Graphics::GetGraphicsBackend())

    FMolecularDynamicsMonitor::FMolecularDynamicsMonitor(const TPointer<Config>& config, Model model)
    : FOutputChannel("Particle dynamics monitor", 10)
    , FSlabWindow(Graphics::FSlabWindowConfig("Particle Dynamics Monitor"))
    , MoleculeShape(CUTOFF_RADIUS, 36)
    , N(config->getN()), L(static_cast<float>(config->GetL()))
    {
        const sf::Color skyBlue(135, 206, 235, 255);
        const sf::Color someRed(235, 206, 135, 255);

        const auto negColor = skyBlue;
        const auto posColor = someRed;

        {
            constexpr int TexDim = 256;

            Pair<double, double> NearestZero = {1.e3, .0},
                    Smallest    = {.0,   .0},
                    Largest     = {.0,   .0};

            MolRenderedPotential.create(TexDim, TexDim, sf::Color(255, 0, 255, 0));
            for (auto i = 0; i < TexDim; ++i) {
                for (auto j = 0; j < TexDim; ++j) {
                    const double x = 2.*(i/static_cast<double>(TexDim) - .5);
                    const double y = 2.*(j/static_cast<double>(TexDim) - .5);
                    const double r = sqrt(x*x + y*y)*CUTOFF_RADIUS;

                    if constexpr (SHOW_RADIUS) if(r>=0.98*CUTOFF_RADIUS)
                        MolRenderedPotential.setPixel(i, j, sf::Color(0, 255, 0, 255));
                    if constexpr (SHOW_DOT) if(r < 0.5*1.12*σ)
                        MolRenderedPotential.setPixel(i, j, sf::Color(255, 255, 255, 255));
                    if constexpr (!SHOW_DOT) {
                        DevFloat U;
                        DevFloat factor=.0;

                        switch (model) {
                        case LennardJones: U = LennardJones::U(r); factor = U/ε; break;
                        case SoftDisk:     U = SoftDisk::    U(r); factor = U;   break;
                        }

                        auto Alpha = 255.0;
                        auto Color = posColor;

                        if(U<0){
                            Color = negColor;
                            Alpha *= factor;
                        }  else {
                            Alpha *= factor>1?1.0:factor;
                        }

                        Color.a = Alpha>255. ? 255 : Alpha<.0 ? 0 : static_cast<UInt8>(Alpha);

                        Color = {0,0,255,255};
                        MolRenderedPotential.setPixel(i, j, Color);

                        if(fabs(U)<fabs(NearestZero.first)) NearestZero={U, r};
                        if(U<Smallest.first)                Smallest   ={U, r};
                        if(U>Largest.first)                 Largest    ={U, r};
                    }
                }
            }

            Log::Info("ParticleDynamics::Monitor: ")
                      << "\n\t\t\t\t\tNearest zero " << NearestZero.first << " @ r=" << NearestZero.second
                      << "\n\t\t\t\t\tSmallest     " << Smallest.first << " @ r=" << Smallest.second
                      << "\n\t\t\t\t\tLargest      " << Largest.first << " @ r=" << Largest.second;
            MolTexture.loadFromImage(MolRenderedPotential);
        }

        MoleculeShape.setTexture(&MolTexture, true);
        if constexpr (SHOW_RADIUS) {
            MoleculeShape.setOutlineThickness(0.1);
            auto color = skyBlue;
            color.a = 50;
            MoleculeShape.setOutlineColor(color);
        }
        MoleculeShape.setOrigin(CUTOFF_RADIUS, CUTOFF_RADIUS);
    }

    void FMolecularDynamicsMonitor::ImmediateDraw(const Graphics::FPlatformWindow& PlatformWindow) {
        auto& RenderWindow = *static_cast<sf::RenderWindow*>(PlatformWindow.GetRawPlatformWindowPointer());

        auto state = LastPacket.GetNakedStateData<FMoleculesState>();
        auto v_q = state->first();
        auto v_p = state->second();

        const auto L_bitMore = (float)L * 1.2f;
        sf::Color bg(0 , 0, 0);
        const sf::Color Azure(135, 206, 235, 32);

        const float ratio = static_cast<float>(RenderWindow.getSize().x) / static_cast<float>(RenderWindow.getSize().y);
        RenderWindow.setView(sf::View(sf::Vector2f(.0, .0), sf::Vector2f(L_bitMore*ratio, -L_bitMore)));
        RenderWindow.clear(bg);

        // Hash grid
        if constexpr (SHOW_HASH_GRID)
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

            RenderWindow.draw(&subdivs[0], subdivs.size(), sf::Lines);
        }


        // Particles
        {
            constexpr auto O = .0f;

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
                    MoleculeShape.setPosition(sf::Vector2f(static_cast<float>(q.x), static_cast<float>(q.y)) + o);
                    if (c == 4)
                        MoleculeShape.setFillColor(sf::Color(255, 255, 255, 80));
                    else
                        MoleculeShape.setFillColor(sf::Color(255, 255, 255, 25));

                    c++;
                    RenderWindow.draw(MoleculeShape);
                }
            }

            // RenderWindow.draw(&MolShapes[0], N, sf::Lines);
        }


        // Boundaries
        {
            sf::Color RosaChoqueDaNina(255, 20, 147);
            sf::Color cor = RosaChoqueDaNina;
            sf::Vertex box[] = {
                    sf::Vertex(sf::Vector2f(-.5f * L,  .5f * L), cor),
                    sf::Vertex(sf::Vector2f( .5f * L,  .5f * L), cor),
                    sf::Vertex(sf::Vector2f( .5f * L, -.5f * L), cor),
                    sf::Vertex(sf::Vector2f(-.5f * L, -.5f * L), cor),
                    sf::Vertex(sf::Vector2f(-.5f * L,  .5f * L), cor)
            };
            RenderWindow.draw(box, 5, sf::LineStrip);
        }


        // Big circle indicating the interaction cutoff radius
        if constexpr (false)
        {
            sf::CircleShape molRadius(CUTOFF_RADIUS);
            molRadius.setPosition(-.5f * L - 3 * CUTOFF_RADIUS, .5f * L - 2 * CUTOFF_RADIUS);
            RenderWindow.draw(molRadius);

            // molShape.setPosition( -.5f * L - 3 * CUTOFF_RADIUS, .5f * L - 5 * CUTOFF_RADIUS);
            // renderWindow.draw(molShape);
        }
    }

    auto FMolecularDynamicsMonitor::HandleOutput(const Math::FOutputPacket&) -> void
    {
        // Dummy
    }
} // MolecularDynamics