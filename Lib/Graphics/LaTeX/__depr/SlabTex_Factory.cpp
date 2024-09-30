//
// Created by joao on 9/26/24.
//

#include "SlabTexGraphics.h"

#include "3rdParty/NanoTeX/src/graphic/graphic.h"
#include "Utils/Exception.h"

#include <string>

namespace tex{
    /**
    * IMPORTANT: do not forget to implement the 2 static methods below,
    * it is the factory methods to create a new font.
    */

    using namespace Slab;

    Font* Font::create(const std::string& file, float size) {
        return new Slab::Graphics::LaTeX::Font_Slab(file, size);
    }

    sptr<Font> Font::_create(const std::string& name, int style, float size) {
        return sptrOf<Slab::Graphics::LaTeX::Font_Slab>(name, style, size);
    }

    sptr<TextLayout> TextLayout::create(const std::wstring& src, const sptr<Font>& font){
        NOT_IMPLEMENTED
    }
}