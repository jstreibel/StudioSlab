//
// Created by joao on 7/25/24.
//

#include "Painter.h"

namespace Slab::Graphics {
    Painter::Painter(const Str &vertFilename, const Str &fragFilename)
    : FShader(vertFilename, fragFilename) {  }

    void Painter::drawGUI() {

    }

    void Painter::Use() const {
        FShader::Use();
    }

} // Slab::Graphics