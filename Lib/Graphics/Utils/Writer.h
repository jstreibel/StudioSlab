//
// Created by joao on 28/08/23.
//

#ifndef STUDIOSLAB_WRITER_H
#define STUDIOSLAB_WRITER_H

#include "Graphics/Window/SlabWindow.h"

#include "Graphics/Styles/Colors.h"
#include "Utils/EncodingUtils.h"

namespace Slab::Graphics {

    class Writer {
    public:
        Writer() = default;
        virtual ~Writer() = default;

        virtual void write(const Str &text, Point2D penLocation, Color color=White, bool vertical=false) = 0;
        virtual DevFloat getFontHeightInPixels() const = 0;
        virtual void reshape(int w, int h) {};

        virtual void scale(float sx, float sy) {};
        virtual void translate(float dx, float dy) {};
        virtual void resetTransforms() {};
    };

}


#endif //STUDIOSLAB_WRITER_H
