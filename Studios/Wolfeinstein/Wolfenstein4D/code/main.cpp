#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "map.h"
#include "player.h"
#include "minimap.h"

#include "ray.h"
#include <cmath>

int winWidth = 1024, winHeight = 768;

const double pi = 3.14159265;
sf::Color bg(86, 110, 73);

const float spd = 1.e-1;
const float angSpd = 3.5e-2;
float angle = 0;

static char *map =
"\
####################\
#                  #\
#  ##    ######    #\
#  #          #    #\
#        ### ##    #\
#  #               #\
#                  #\
#                  #\
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

    sf::RenderWindow window(sf::VideoMode(winWidth, winHeight), "Wolfenstein (3+1)D, bitches!");
    window.setFramerateLimit(60);

    Minimap minimap(defaultMap);

    std::vector<Ray> rays(100);
    for(int i=0; i<rays.size(); i++){
        rays[i].pos = Vector2f(4.5, 6.5);
        rays[i].angle = (float(i)/rays.size())*pi/4;
        const float c = 2.e-2;
        rays[i].c = Vector2f(c,c);
        minimap.addRay(rays[i]);
    }

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event)){
            if (event.type == sf::Event::Closed) window.close();
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
            for(Ray &ray in rays) ray.angle-=angSpd;
            angle -= angSpd;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
            for(Ray &ray in rays) ray.angle+=angSpd;
            angle += angSpd;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            for(Ray &ray in rays){
                Vector2f n(cos(angle), sin(angle));
                ray.pos += Vector2f(n.x*spd, n.y*spd);
            }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            for(Ray &ray in rays){
                Vector2f n(cos(angle), sin(angle));
                ray.pos -= Vector2f(n.x*spd, n.y*spd);
            }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            for(Ray &ray in rays){
                Vector2f n(-sin(angle), cos(angle));
                ray.pos -= Vector2f(n.x*spd, n.y*spd);
            }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            for(Ray &ray in rays){
                Vector2f n(-sin(angle), cos(angle));
                ray.pos += Vector2f(n.x*spd, n.y*spd);
            }

        window.clear(bg);
        minimap.draw(window);
        window.display();
    }

    return 0;
}
