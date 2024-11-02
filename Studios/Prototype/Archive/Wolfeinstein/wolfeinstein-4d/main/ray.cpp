#include "ray.h"

#include "block-intersector.h"

#include <cmath>

#include <cstdio>

Ray::Ray(Vector2f pos, float angle, Vector2f c) :pos(pos), angle(angle), c(c)
{


}

float Ray::checkCollisionQuad(Block &bl)
{
    Vector2f norm(cos(angle), sin(angle));
    float n_x = norm.x, n_y = norm.y;

    const float &x = pos.x, &y = pos.y;
    const float xMin = bl.x-.5,
                xMax = bl.x+.5,
                yMin = bl.y-.5,
                yMax = bl.y+.5;

    // Is within?
    if(pos.x <= xMax && pos.x >= xMin && pos.y >= yMin && pos.y <= yMax)
        return -1;

    float tCol = 1.e30;

    float t;
    float tp;
    float b_x = bl.x, b_y = bl.y;
    float c_x = this->c.x, c_y = this->c.y;
    {
        t = tA(b_y, c_y, n_y, y);
        tp = tpA(b_x, b_y, c_x, c_y, n_x, n_y, x, y);
    }
    if(t>0 && tp>=0 && tp<=1 && t<tCol) tCol = t;
    {
        t = tB(b_x, c_x, n_x, x);
        tp = tpB(b_x, b_y, c_x, c_y, n_x, n_y, x, y);
    }
    if(t>0 && tp>=0 && tp<=1 && t<tCol) tCol = t;
    {
        t = tC(b_y, c_y, n_y, y);
        tp = tpC(b_x, b_y, c_x, c_y, n_x, n_y, x, y);
    }
    if(t>0 && tp>=0 && tp<=1 && t<tCol) tCol = t;
    {
        t = tD(b_x, c_x, n_x, x);
        tp = tpD(b_x, b_y, c_x, c_y, n_x, n_y, x, y);
    }
    if(t>0 && tp>=0 && tp<=1 && t<tCol) tCol = t;

    return tCol;
}

// Sx Sy a b
int pSet[4][4] = {
    {-1, -1,  1,  0},
    {+1, -1,  0,  1},
    {+1, +1, -1,  0},
    {-1, +1,  0, -1},
};

float Ray::checkCollisionLinear(Block &b)
{
    Vector2f n(cos(angle), sin(angle));

    const float &x = pos.x, &y = pos.y;
    const float xMin = b.x-.5,
                xMax = b.x+.5,
                yMin = b.y-.5,
                yMax = b.y+.5;

    // Is within?
    if(pos.x <= xMax && pos.x >= xMin && pos.y >= yMin && pos.y <= yMax)
        return -1;

    float tCol = 1.e30;
    for(int i=0; i<4; i++){
        const int *p = pSet[i];
        const int Sx=p[0], Sy=p[1], ax=p[2], ay=p[3];

        float t;
        float tp;

        if(ax&1){ // if a!=0
            t = (1./n.y)*(b.y-y+.5*Sy);
            tp = ax*(x+t*n.x-b.x-.5*Sx);
        }
        else{
            t = (1./n.x)*(b.x-x+.5*Sx);
            tp = ay*(y+t*n.y-b.y-.5*Sy);
        }

        if(t>0 && tp>=0 && tp<=1 && t<tCol) // We have intersection.
            tCol = t;
    }

    return tCol;
}

float Ray::cast(Map &map)
{
    BlockSet &blocks = map.blocks;

    float t=1.e30;
    for(auto bl : blocks){
        //float tb = checkCollisionQuad(*bl);
        float tb = checkCollisionLinear(*bl);
        if(tb > 0 && tb<t) t=tb;
    }

    return t;
}

Vector2f Ray::n()
{
    return Vector2f(cos(angle), sin(angle));
}
