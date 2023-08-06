#ifndef OUTPUTOPENGL_H
#define OUTPUTOPENGL_H

#include "IOParams.h"

#include "KeyPressListener.h"
#include "Hamiltonians/Types.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


class BackendSFML : public KeyPressListener
{
	sf::RenderWindow mWindow;

    std::vector<sf::Vertex> molShapes;

	sf::CircleShape molShape;
	sf::Texture molTexture;

	std::vector<sf::Vertex[MOLS_HISTORY_SIZE]> moleculesHistory;

	bool run = true;

public:
    BackendSFML();

    void operator()(const PointContainer &v_q, bool *flippedSides);

    void KeyPressed(sf::Event::KeyEvent &keyEvent, int value) override;

	bool shouldRun() const;
private:
};

#endif // OUTPUTOPENGL_H
