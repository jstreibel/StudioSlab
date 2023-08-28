//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_ARTIST_H
#define V_SHAPE_ARTIST_H

class Window;

class Artist {
public:
    virtual void draw(const Window *window) const = 0;

    virtual void reshape(int w, int h) {};
};


#endif //V_SHAPE_ARTIST_H
