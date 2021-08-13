#include "player.h"


Player::Player(sf::Vector2f vPos, float fSpeed, float fAngle, float fAngleSpeed)
    : vPos(vPos), fSpeed(fSpeed), fAngle(fAngle), fAngleSpeed(fAngleSpeed)
{

}

sf::Vector2f Player::getNorm()
{
    return sf::Vector2f(cos(fAngle), sin(fAngle));
}

sf::Vector2f Player::getTan()
{
    return sf::Vector2f(-sin(fAngle), cos(fAngle));
}


