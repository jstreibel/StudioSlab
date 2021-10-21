#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <SFML/Graphics/RenderWindow.hpp>

#include "map.h"
#include "player.h"

class RayCaster
{
private:
    //RayCaster() {}
public:
    RayCaster(Map *map, Player *player);

    void draw(sf::RenderWindow &window);

private:
    Map *cMap;
    Player *cPlayer;

};

#endif // RAYCASTER_H
