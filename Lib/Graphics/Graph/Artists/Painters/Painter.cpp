//
// Created by joao on 7/25/24.
//

#include "Painter.h"

namespace Slab::Graphics {
    Painter::Painter(const Str &vertFilename, const Str &fragFilename)
    : Shader(vertFilename, fragFilename) {  }

    void Painter::drawGUI() {

    }

    void Painter::use() const {
        Shader::use();
    }

} // Slab::Graphics