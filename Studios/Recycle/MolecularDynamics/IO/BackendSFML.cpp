
#include "BackendSFML.h"
#include "UserInput.h"

#include "Hamiltonians/Lennard-Jones/LennardJones.h"
#include "Hamiltonians/Lennard-Jones/LennardJonesParams.h"

#include "Hamiltonians/NewtonMechanicsParams.h"

#include <SFML/OpenGL.hpp>

#define RAD2DEG 57.2957795131



BackendSFML::BackendSFML()
    : mWindow(new sf::RenderWindow(sf::VideoMode(1920, 1080), "Molecular dynamic"
                                                        /*, sf::Style::Fullscreen*/)),
      molShapes(2*N_MOLS), molShape(CUTOFF_RADIUS, 36), molTexture(), moleculesHistory(N_MOLS)
{
    UserInput::Initialize(mWindow);

    mWindow->setFramerateLimit(0);
    mWindow->setPosition(sf::Vector2i(250, 250));

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

                    if(i==texDim/2 && j>=texDim/2) std::cout << "\nU(" << r << ")/eps=" << factor;

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

    bool a = false;
    for(sf::Vertex *track : moleculesHistory) {
        for (int i = 0; i < MOLS_HISTORY_SIZE; i++) {
            const float factor = (float(i + 1) / MOLS_HISTORY_SIZE);
            const sf::Uint8 val = sf::Uint8(128 * factor);
            track[i] = sf::Vertex({0, 0},
                                  {255, 255, 255, val});
        }
        a = true;
    }
}

void BackendSFML::operator()(const PointContainer &v_q, bool *flippedSides) {
    const double l = SPACE_L * 1.2;
    sf::Color bg(0 , 0, 0);
    sf::Color skyBlue(135, 206, 235, 32);

    const float ratio = mWindow->getSize().x / float(mWindow->getSize().y);
    mWindow->setView(sf::View(sf::Vector2f(.0, .0), sf::Vector2f(l*ratio, -l)));
    mWindow->clear(bg);

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

        mWindow->draw(&subdivs[0], subdivs.size(), sf::Lines);
    }


    {
        int i = 0;
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
                mWindow->draw(moleculeShape);
            }

            sf::Vertex *myHistory = moleculesHistory[i];
            myHistory[MOLS_HISTORY_SIZE - 1].position = {float(q.x), float(q.y)};
            if (flippedSides[i]) {
                for (int i = 0; i < MOLS_HISTORY_SIZE - 1; i++)
                    myHistory[i].position = {float(q.x), float(q.y)};
                flippedSides[i] = false;
            } else {
                for (int i = 0; i < MOLS_HISTORY_SIZE - 1; i++)
                    myHistory[i].position = myHistory[i + 1].position;
            }
            mWindow->draw(myHistory, MOLS_HISTORY_SIZE, sf::LineStrip);

            i++;
        }
        //mWindow->draw(&molShapes[0], N_MOLS, sf::Lines);
    }

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
        mWindow->draw(box, 5, sf::LineStrip);
    }

    // Grandao
    {
        sf::CircleShape molRadius(CUTOFF_RADIUS);
        molRadius.setPosition(-.5 * L - 3 * CUTOFF_RADIUS, .5 * L - 2 * CUTOFF_RADIUS);
        mWindow->draw(molRadius);
    }

    mWindow->display();

}

void BackendSFML::KeyPressed(sf::Event::KeyEvent &keyEvent, int value) {
    switch (keyEvent.code){
        case sf::Keyboard::Left:
            break;
        case sf::Keyboard::Right:
            break;
        case sf::Keyboard::Up:
            break;
    }
}


