#ifndef BLOCK_H
#define BLOCK_H

class Block
{
public:
    Block(float x, float y) : x(x), y(y) {}

    float x, y;
};

typedef Vector<Block*> BlockSet;

#endif // BLOCK_H
