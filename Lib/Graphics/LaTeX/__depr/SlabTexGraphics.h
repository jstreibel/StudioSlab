//
// Created by joao on 9/26/24.
//

#ifndef STUDIOSLAB_SLABTEXGRAPHICS_H
#define STUDIOSLAB_SLABTEXGRAPHICS_H

#include "3rdParty/NanoTeX/src/graphic/graphic.h"
#include "3rdParty/NanoTeX/src/utils/utils.h"
#include "3rdParty/NanoTeX/src/graphic/graphic_basic.h"

#include "Utils/String.h"
#include "Utils/Pointer.h"

#include "Graphics/OpenGL/Writer.h"


namespace Slab::Graphics::LaTeX {

    class Font_Slab : public tex::Font {
        int w, h;
        Pointer<Writer> writer;
    public:
        Pointer<Writer> getWriter();


        Font_Slab(const Str& file, float size);

        Font_Slab(const Str& name, int style, float size);

        // ... implementations of the other methods ...

        float getSize() const override;

        tex::sptr <tex::Font> deriveFont(int style) const override;

        bool operator==(const tex::Font &f) const override;

        bool operator!=(const tex::Font &f) const override;

        void reshape(int w, int h);
    };

    class TextLayout_Slab : public tex::TextLayout {
    public:
        void getBounds(tex::Rect &bounds) override;

        void draw(tex::Graphics2D &g2, float x, float y) override;
    };

    class Graphics2D_Slab : public tex::Graphics2D {
        Font_Slab* current_font = nullptr;
        tex::Stroke current_stroke;
        tex::color current_color{};
        int win_w{}, win_h{};
        float scale_x, scale_y;

    public: // StudioSlab requirement:
        void reshape(int new_win_w, int new_win_h);

    public:
        void setColor(tex::color c) override;

        tex::color getColor() const override;

        void setStroke(const tex::Stroke &s) override;

        const tex::Stroke &getStroke() const override;

        void setStrokeWidth(float w) override;

        const tex::Font *getFont() const override;

        void setFont(const tex::Font *font) override;

        void translate(float dx, float dy) override;

        void scale(float sx, float sy) override;

        void rotate(float angle) override;

        void rotate(float angle, float px, float py) override;

        void reset() override;

        float sx() const override;

        float sy() const override;

        void drawChar(wchar_t c, float x, float y) override;

        void drawText(const std::wstring &c, float x, float y) override;

        void drawLine(float x1, float y1, float x2, float y2) override;

        void drawRect(float x, float y, float w, float h) override;

        void fillRect(float x, float y, float w, float h) override;

        void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override;

        void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override;
    };
}

#endif //STUDIOSLAB_SLABTEXGRAPHICS_H
