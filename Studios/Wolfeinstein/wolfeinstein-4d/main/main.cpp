#include <iostream>
#include <sstream>
#include <string>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "math_defs.h"

#include "map.h"
#include "player.h"
#include "minimap.h"
#include "raycaster.h"

int iWinWidth = 1024, iWinHeight = 768;
std::string strWinTitle = "Wolfeinstein (3+1)D, bitches!";

const float fAtt=0.3;
sf::Color bg(fAtt*86, fAtt*110, fAtt*73);

static char *map =
"\
####################\
#                  #\
#  ##    ######    #\
#  #          #    #\
#        ### ##    #\
#  #               #\
#                  #\
#        #         #\
#                  #\
#                  #\
#  #         #     #\
#                  #\
#                  #\
#                  #\
####################";
Map defaultMap(20, 15, map);

int main()
{
    if(!defaultMap.makeSureItsFine()) return -1;

    sf::RenderWindow window(sf::VideoMode(iWinWidth, iWinHeight), strWinTitle);
    window.setFramerateLimit(60);


    Minimap cMinimap(defaultMap);
    Player cPlayer(sf::Vector2f(2.5, 6.5));
    cMinimap.addPlayer(&cPlayer);
    RayCaster cRayCaster(&defaultMap, &cPlayer);

    sf::Clock cClock;

    while(window.isOpen())
    {
        float fFPS = 1.f / cClock.restart().asSeconds();

        std::ostringstream strTitleFPS;
        strTitleFPS << strWinTitle << "  -  [" << fFPS << " FPS]";
        window.setTitle(strTitleFPS.str());


        sf::Event event;
        while(window.pollEvent(event))
            if (event.type == sf::Event::Closed) window.close();

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            cPlayer.fAngle -= cPlayer.fAngleSpeed;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            cPlayer.fAngle += cPlayer.fAngleSpeed;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            cPlayer.vPos += cPlayer.fSpeed * cPlayer.getNorm();

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            cPlayer.vPos -= cPlayer.fSpeed * cPlayer.getNorm();

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            cPlayer.vPos -= cPlayer.fSpeed * cPlayer.getTan();

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            cPlayer.vPos += cPlayer.fSpeed * cPlayer.getTan();

        window.clear(bg);
        cRayCaster.draw(window);
        cMinimap.draw(window);
        window.display();
    }

    return 0;
}
