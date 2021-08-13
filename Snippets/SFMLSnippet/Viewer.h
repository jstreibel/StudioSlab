//
// Created by joao on 25/05/2021.
//

#ifndef SFMLSNIPPET_VIEWER_H
#define SFMLSNIPPET_VIEWER_H

#include <SFML/Graphics.hpp>

class Viewer {
    sf::RenderWindow window;

    sf::Image bitmap;
    sf::Texture textura;
    sf::Sprite sprite;

    bool isTextureValid=false;
public:
    Viewer(int w, int h, const sf::String& nomeDaJanela) : window(sf::VideoMode(w, h), nomeDaJanela) {
        window.setFramerateLimit(0);
        bitmap.create(w, h);

        for (int i = 0; i < w; ++i)
            for (int j = 0; j < h; ++j)
                bitmap.setPixel(i, j, sf::Color::Black);
        textura.loadFromImage(bitmap);
        isTextureValid = true;

        sprite.setTexture(textura, true);


        sprite.setScale(1, 1);
        sprite.setPosition(0, 0);
    }

    void trataEventos() {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                exit(0);
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                    exit(0);
                }
            }
        }
    }

    void setPixel(int x, int y, sf::Color color){
        bitmap.setPixel(x, y, color);
        isTextureValid = false;
    }

    void setPixelEntreZeroEUm(float xNorm, float yNorm, sf::Color color){
        int x = xNorm*window.getSize().x, y=yNorm*window.getSize().y;
        this->setPixel(x, y, color);
    }

    virtual void desenhaTudo() {
        if(!isTextureValid){
            textura.loadFromImage(bitmap);
            isTextureValid = true;
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
};

#endif //SFMLSNIPPET_VIEWER_H
