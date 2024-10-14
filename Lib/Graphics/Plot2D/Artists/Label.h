//
// Created by joao on 01/06/23.
//

#ifndef STUDIOSLAB_LABEL_H
#define STUDIOSLAB_LABEL_H

#include <utility>

#include "Utils/Types.h"
#include "Artist.h"

namespace Slab::Graphics {

    enum LocationReference {
        SystemWindow,
        Viewport,
        Space
    };

    struct Label : Artist {
        Point2D pos;
        LocationReference reference;

        Label(Point2D pos, LocationReference reference)
                : pos(pos), reference(reference) {}

        void draw(const Window *window) const override {

        }
    };

}

#endif //STUDIOSLAB_LABEL_H
