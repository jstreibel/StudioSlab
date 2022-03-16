#ifndef OUTPUTOPENGL_H
#define OUTPUTOPENGL_H

#include "IOParams.h"

#include "KeyPressListener.h"
#include "Hamiltonians/Types.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


class BackendSFML : public KeyPressListener
{
public:
    BackendSFML();

    void operator()(const PointContainer &v_q, bool *flippedSides);

    void KeyPressed(sf::Event::KeyEvent &keyEvent, int value) override;

private:
    std::vector<sf::Vertex> molShapes;

	sf::RenderWindow *mWindow;

	sf::CircleShape molShape;
	sf::Texture molTexture;


	std::vector<sf::Vertex[MOLS_HISTORY_SIZE]> moleculesHistory;
};

#endif // OUTPUTOPENGL_H
