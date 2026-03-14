//
// Created by joao on 28/08/23.
//

#ifndef STUDIOSLAB_WRITER_H
#define STUDIOSLAB_WRITER_H

#include "Graphics/Window/SlabWindow.h"

#include "Graphics/Styles/Colors.h"
#include "Utils/EncodingUtils.h"

namespace Slab::Graphics {

    class FWriter {
    public:
        FWriter() = default;
        virtual ~FWriter() = default;

        virtual void Write(const Str &text, FPoint2D penLocation, FColor color=White, bool vertical=false) = 0;
        [[nodiscard]] virtual DevFloat GetFontHeightInPixels() const = 0;
        [[nodiscard]] virtual DevFloat GetLineAdvanceInPixels() const { return GetFontHeightInPixels(); }
        [[nodiscard]] virtual DevFloat MeasureTextWidthInPixels(const Str &text) const {
            return static_cast<DevFloat>(text.size()) * (0.6 * GetFontHeightInPixels());
        }
        virtual void Reshape(int w, int h) {};

        virtual void Scale(float sx, float sy) {};
        virtual void Translate(float dx, float dy) {};
        virtual void ResetTransforms() {};
    };

}


#endif //STUDIOSLAB_WRITER_H
