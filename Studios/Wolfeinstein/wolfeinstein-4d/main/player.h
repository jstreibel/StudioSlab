#ifndef PLAYER_H
#define PLAYER_H

#include <cmath>
#include <SFML/System/Vector2.hpp>

#include "math_defs.h"

struct Player {
    static const char symbol = 'P';

    Player(sf::Vector2f vPos, float fSpeed = .5e-1, float fAngle = -pi/2, float fAngleSpeed = 3.5e-2);

    sf::Vector2f getNorm();
    sf::Vector2f getTan();

    sf::Vector2f vPos;
    float fSpeed;
    float fAngleSpeed;
    float fAngle;
    float fFoV = pi/4;
};


#endif // PLAYER_H
