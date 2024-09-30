//
// Created by joao on 9/26/24.
//

#include "SlabTexGraphics.h"

#include "Core/Tools/Resources.h"
#include "Core/Tools/Log.h"

namespace Slab::Graphics::LaTeX {

#define TEST Log::Info() << __PRETTY_FUNCTION__ << Log::Flush
//#define TEST


    using namespace Core;

    const float base_scale = 40;

    Font_Slab::Font_Slab(const Str &file, float size) {
        // load platform-specific font from given file and size
        auto full_filename = Core::Resources::NanoTeXFolder + file;
        Log::Status("Load font file '") << file << "', size " << size << "." << Log::Flush;

        writer = New<Writer>(full_filename, size*base_scale);
    }

    Font_Slab::Font_Slab(const Str &name, int style, float size) {
        // create platform-specific font with given name, style
        // and size

        Log::Status("Load font name '") << name << "', style '" << style << "', size " << size << "." << Log::Flush;
        if(name == "Serif"){
            writer = New<Writer>(Core::Resources::fontFileName(11), size*base_scale);
        } else {
            writer = New<Writer>(Core::Resources::fontFileName(10), size*base_scale);
        }
    }

    float Font_Slab::getSize() const {
        return (float)writer->getFontHeightInPixels();
    }

    tex::sptr<tex::Font> Font_Slab::deriveFont(int style) const {
        Core::Log::Info() << "Deriving font style " << style << Core::Log::Flush;
        return tex::sptr<LaTeX::Font_Slab>();
    }

    bool Font_Slab::operator==(const tex::Font &f) const {
        TEST;
        return false;
    }

    bool Font_Slab::operator!=(const tex::Font &f) const {
        TEST;
        return false;
    }

    void Font_Slab::reshape(int w_, int h_) {
        this->w = w_;
        this->h = h_;
        writer->reshape(w, h);
    }

    Pointer<Writer> Font_Slab::getWriter() {
        if(writer!=nullptr)
            writer->reshape(w, h);

        return writer;
    }







    void TextLayout_Slab::getBounds(tex::Rect &bounds) {
        TEST;
    }

    void TextLayout_Slab::draw(tex::Graphics2D &g2, float x, float y) {
        TEST;
    }




    void Graphics2D_Slab::reshape(int w_, int h_) {
        win_w=w_; win_h=h_;
        if(current_font) current_font->reshape(w_, h_);
    }

    void Graphics2D_Slab::setColor(tex::color c) {
        current_color = c;
    }

    tex::color Graphics2D_Slab::getColor() const {
        return 0xffffffff;
    }

    void Graphics2D_Slab::setStroke(const tex::Stroke &s) {
        current_stroke = s;
    }

    const tex::Stroke &Graphics2D_Slab::getStroke() const {
        return current_stroke;
    }

    void Graphics2D_Slab::setStrokeWidth(float w) {
        current_stroke.lineWidth = w;
    }

    const tex::Font *Graphics2D_Slab::getFont() const {
        return current_font;
    }

    void Graphics2D_Slab::setFont(const tex::Font *font) {
        current_font = (Font_Slab*)font;
        current_font->reshape(win_w, win_h);
    }

    void Graphics2D_Slab::translate(float dx, float dy) {
        if(current_font == nullptr) return;

        auto writer = current_font->getWriter();

        if(writer != nullptr) {
            writer->translate(dx*base_scale, dy*base_scale);
        }
    }

    void Graphics2D_Slab::scale(float sx, float sy) {
        if(current_font == nullptr) return;

        auto writer = current_font->getWriter();

        if(writer != nullptr) {
            writer->scale(sx/base_scale, sy/base_scale);
        }

        scale_x *= sx/base_scale;
        scale_y *= sy/base_scale;
    }

    void Graphics2D_Slab::rotate(float angle) {
        Core::Log::Info() << "Rotate " << angle << Core::Log::Flush;
    }

    void Graphics2D_Slab::rotate(float angle, float px, float py) {
        Core::Log::Info() << "Rotate " << angle << " around (" << px << "," << py << ")" << Core::Log::Flush;
    }

    void Graphics2D_Slab::reset() {
        if(current_font == nullptr) return;

        auto writer = current_font->getWriter();

        if(writer != nullptr)
            writer->resetTransforms();

        scale_x=1;
        scale_y=1;
    }

    float Graphics2D_Slab::sx() const {
        TEST;
        NOT_IMPLEMENTED_CLASS_METHOD
    }

    float Graphics2D_Slab::sy() const {
        TEST;
        NOT_IMPLEMENTED_CLASS_METHOD
    }

    void Graphics2D_Slab::drawChar(wchar_t c, float x, float y) {
        drawText(WStr(1,c), x, y);
    }

    void Graphics2D_Slab::drawText(const std::wstring &text, float x, float y) {
        if(current_font == nullptr) return;

        auto writer = current_font->getWriter();

        if(writer != nullptr)
            writer->write(WStrToStr(text), {x,y});
    }

    void Graphics2D_Slab::drawLine(float x1, float y1, float x2, float y2) {
        TEST;
    }

    void Graphics2D_Slab::drawRect(float x, float y, float w, float h) {
        TEST;
    }

    void Graphics2D_Slab::fillRect(float x, float y, float w, float h) {
        TEST;
    }

    void Graphics2D_Slab::drawRoundRect(float x, float y, float w, float h, float rx, float ry) {
        TEST;
    }

    void Graphics2D_Slab::fillRoundRect(float x, float y, float w, float h, float rx, float ry) {
        TEST;
    }


}