#ifndef RAY_H
#define RAY_H

#include <SFML/System/Vector2.hpp>
using namespace sf;

#include "map.h"

class Ray
{
public:
    Ray(){}
    Ray(Vector2f pos, float angle, Vector2f c);

    Vector2f pos;
    float angle;
    Vector2f c;

    float checkCollisionQuad(Block &bl);
    float checkCollisionLinear(Block &block);

    /**
     * @brief cast
     * @param map The map to cast the ray into.
     * @return Position of nearest hit.
     */
    float cast(Map &map);

    Vector2f n();
};

#endif // RAY_H
