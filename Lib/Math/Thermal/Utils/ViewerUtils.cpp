//
// Created by joao on 19/09/2021.
//

#include "ViewerUtils.h"

bool ViewerUtils::LoadFont(sf::Font &font){
    if (!font.loadFromFile(ViewerUtils::fontFile))
        throw "SFML error while loading font.";

    return true;
}