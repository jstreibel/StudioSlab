#ifndef MINIMAP_H
#define MINIMAP_H

#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>

#include "map.h"
#include "ray.h"

typedef std::vector<sf::Shape*> ShapeSet;
typedef std::vector<sf::Drawable*> DrawableSet;

using namespace sf;

class Minimap {
public:
    Minimap(Map &map)
        : map(map)
    {
        this->cellW = 32.;
        this->cellH = 32.;
        Uint32 minimapW = cellW*map.hCells,
               minimapH = cellH*map.vCells;

        Vector2f cellSize(cellW, cellH);

        wallBlock = new RectangleShape(cellSize);
        //pwallBlock->setFillColor(Color(100, 131, 124));
        wallBlock->setFillColor(Color::Black);
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
        Vector2f myPos(0,0);

        for(int i=0; i<map.hCells; i++){
            for(int j=0; j<map.vCells; j++){
                Shape *shape = shapes[i+j*map.hCells];
                Vector2f relPos(i*cellW, j*cellH);
                Vector2f absPos(myPos+relPos);
                shape->setPosition(absPos);
                window.draw(*shape);
            }
        }

        for(Ray* r : rays){
            Ray &ray = *r;

            Vector2f pos = ray.pos*cellW;

            CircleShape loc (3, 10);
            loc.setPosition(pos);
            loc.setFillColor(sf::Color::Cyan);

            window.draw(loc);

            if(1)
            {
                float t = ray.cast(map);
                const int N = 20;
                VertexArray line(sf::LineStrip, N);
                const float dt = t/float(N-1);
                for(int i=0; i<N; i++){
                    const float s = i*dt;
                    line[i].position = pos + ray.n()*s*cellW + ray.c*s*s*cellW;
                }

                window.draw(line);
            } else {
                float t = ray.cast(map);
                VertexArray line(sf::Lines, 10);
                line[0].position = pos;
                line[1].position = pos + ray.n()*t*cellW;

                window.draw(line);
            }
        }
    }

    void addRay(Ray &ray){
        rays.push_back(&ray);
    }

private:
    Map &map;

    float cellW, cellH;
    Shape *wallBlock, *floorBlock, *invBlock, *playerBlock;

    std::vector<Ray*> rays;

    ShapeSet blocks;
    ShapeSet shapes;
};

#endif // MINIMAP_H
