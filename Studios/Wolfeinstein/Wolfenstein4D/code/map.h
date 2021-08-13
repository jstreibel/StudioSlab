#ifndef MAP_H
#define MAP_H

#include <vector>

#define each auto
#define in :

#include "block.h"

struct Map
{
    /**
     * @brief Map
     * @param horizontalCells Horizontal size of map in cells.
     * @param vertCells Vertical size of map in cells.
     * @param map The map per se.
     * @param scale Cell size in meters.
     */
    Map(int horizontalCells, int vertCells, char *map)
        : hCells(horizontalCells), vCells(vertCells), map(map)
    {
        for(int i=0; i<horizontalCells; i++)
            for(int j=0; j<vertCells; j++)
                if(map[i+j*horizontalCells] == '#')
                    blocks.push_back(new Block(float(i) + .5,
                                               float(j) + .5));
    }

    ~Map() {
        for(each *b in blocks) delete b;
    }

    int hCells, vCells;
    char *map;
    BlockSet blocks;

    char operator [] (const size_t index){ return map[index]; }

    bool makeSureItsFine(){
        int N=0;
        Map &map = *this;
        for(; map[N]!= '\0'; N++);

        return N == hCells*vCells;
    }

};

#endif // MAP_H
