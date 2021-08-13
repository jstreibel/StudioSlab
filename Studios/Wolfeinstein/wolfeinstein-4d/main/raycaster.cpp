#include "raycaster.h"

#include <SFML/Graphics/VertexArray.hpp>
#include "ray.h"

#define LIGHT_INTENSITY 255

RayCaster::RayCaster(Map *map, Player *player) : cMap(map), cPlayer(player)
{

}

void RayCaster::draw(sf::RenderWindow &window)
{
    const sf::Uint32 uXRes = window.getSize().x;
    const sf::Uint32 uYRes = window.getSize().y;
    sf::VertexArray vaLine(sf::Lines, 2);

    float fAngMin = cPlayer->fAngle - cPlayer->fFoV*.5f;
    float fDAng = cPlayer->fFoV / uXRes;

    Ray cRay(cPlayer->vPos, 0, Vector2f(0.00,0.00));
    for(sf::Uint32 i=0; i<uXRes; i++){
        cRay.angle = fAngMin + float(i)*fDAng;
        float z = cRay.cast(*cMap);

        float fBlockScreenHeight = uYRes/z;
        vaLine[0].position = Vector2f(i, .5*(uYRes-fBlockScreenHeight));
        vaLine[1].position = Vector2f(i, .5*(uYRes+fBlockScreenHeight));
        vaLine[0].color = sf::Color(LIGHT_INTENSITY/z, LIGHT_INTENSITY/z, LIGHT_INTENSITY/z);
        vaLine[1].color = sf::Color(LIGHT_INTENSITY/z, LIGHT_INTENSITY/z, LIGHT_INTENSITY/z);

        window.draw(vaLine);
    }
}
