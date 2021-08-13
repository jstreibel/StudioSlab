#ifndef CANVAS_H
#define CANVAS_H

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

using namespace sf;

class Canvas : public Sprite
{
public:
    Canvas() : Sprite()
    {
        Uint32 canvasW = 128, canvasH = 128;

        pixels = new Uint8[canvasW*canvasH*4];
        texture.create(canvasW, canvasH);
        this->setTexture(texture, true);

        for(int i=0; i<canvasW*canvasH*4; i++){
            pixels[i] = 0;
        }

        this->update();
    }

    void update() {
        texture.update(pixels);
    }

private:

    Uint8 *pixels;
    Texture texture;
};

#endif // CANVAS_H
