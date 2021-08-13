#ifndef MINIMAP_H
#define MINIMAP_H

#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>

#include "map.h"
#include "ray.h"
#include "player.h"

#define RAY_RESOLUTION 2
#define NUM_RAYS 15

typedef std::vector<sf::Shape*> ShapeSet;
typedef std::vector<sf::Drawable*> DrawableSet;

using namespace sf;

class Minimap {
public:
    Minimap(Map &map)
        : map(map)
    {
        this->iCellScale = 16;
        vMinimapSize = Vector2u(iCellScale*map.hCells, iCellScale*map.vCells);

        Vector2f cellSize(iCellScale, iCellScale);

        wallBlock = new RectangleShape(cellSize);
        wallBlock->setFillColor(Color(100, 131, 124));
        //wallBlock->setFillColor(Color::White);
        blocks.push_back(wallBlock);
        floorBlock = new RectangleShape(cellSize);
        floorBlock->setFillColor(Color::Black);
        blocks.push_back(floorBlock);
        invBlock = new RectangleShape(cellSize);
        invBlock->setFillColor(Color::Red);
        blocks.push_back(invBlock);
        playerBlock = new RectangleShape(cellSize);
        playerBlock->setFillColor(Color::Yellow);
        blocks.push_back(playerBlock);

        shapes.resize(map.hCells*map.vCells, invBlock);

        for(int i=0; i<map.hCells; i++){

            for(int j=0; j<map.vCells; j++){
                int index = i+j*map.hCells;
                Shape *block = invBlock;

                switch (map[index]) {
                case 'P':
                    block = playerBlock;
                    break;
                case '#':
                    block = wallBlock;
                    break;
                case ' ':
                    block = floorBlock;
                    break;
                default:
                    break;
                }
                shapes[index] = block;
            }
        }
    }

    ~Minimap(){
        for (auto &block : blocks) delete block;
    }

    void draw(RenderWindow &window){
        const Vector2u vWindowPos = window.getSize();
        Vector2u myPos = Vector2u(vWindowPos.x - vMinimapSize.x - 16, 16);

        for(int i=0; i<map.hCells; i++){
            for(int j=0; j<map.vCells; j++){
                Shape *shape = shapes[i+j*map.hCells];
                Vector2u relPos(i*iCellScale, j*iCellScale);
                Vector2f absPos(myPos+relPos);
                shape->setPosition(absPos);
                window.draw(*shape);
            }
        }

        sf::Vector2f vPlayerPos = cPlayer->vPos;
        sf::Vector2f vPlayerPosTransf = float(iCellScale) * vPlayerPos + Vector2f(myPos);

        float fPlayerAngle = cPlayer->fAngle;
        float fPlayerFoV = cPlayer->fFoV;
        float fDAng = fPlayerFoV/ NUM_RAYS;

        CircleShape loc (3, 10);
        loc.setFillColor(sf::Color::Cyan);
        loc.setPosition(vPlayerPosTransf);

        window.draw(loc);

        Ray cRay(vPlayerPos, fPlayerAngle, sf::Vector2f(0.00,0.00));
        float fov2 = 0.5 * fPlayerFoV;
        for (float angle = fPlayerAngle - fov2; angle < fPlayerAngle + fov2; angle += fDAng)
        {
            cRay.angle = angle;

            float t = cRay.cast(map);
            if(1){
                const int N = 20;
                VertexArray line(sf::LineStrip, N);
                const float dt = t/float(N-1);
                for(int i=0; i<N; i++){
                    const float s = i*dt;
                    line[i].position = vPlayerPosTransf + (s*iCellScale)*cRay.n() + (s*s*iCellScale)*cRay.c;
                }
                window.draw(line);
            } else {
                VertexArray line(sf::Lines, RAY_RESOLUTION);
                line[0].position = vPlayerPosTransf;
                line[1].position = vPlayerPosTransf + (t*iCellScale)*cRay.n();
                window.draw(line);
            }
        }
    }


    void addPlayer(Player *player)
    {
        cPlayer = player;
    }

private:
    Map &map;

    Vector2u vMinimapSize;

    Uint32 iCellScale;
    Shape *wallBlock, *floorBlock, *invBlock, *playerBlock;

    std::vector<Ray*> rays;

    ShapeSet blocks;
    ShapeSet shapes;
    Player *cPlayer;
};

#endif // MINIMAP_H
