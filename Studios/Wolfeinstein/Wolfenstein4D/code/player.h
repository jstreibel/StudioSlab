#ifndef PLAYER_H
#define PLAYER_H

struct Player {
    static const char symbol = 'P';

    float posX, posY;
    float velX, velY;
    float angle, fov;
};


#endif // PLAYER_H
