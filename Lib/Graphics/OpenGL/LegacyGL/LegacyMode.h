//
// Created by joao on 8/13/25.
//

#ifndef LEGACYMODE_H
#define LEGACYMODE_H

#include "Graphics/OpenGL/Utils.h"

namespace Slab::Graphics::OpenGL::Legacy {

    /**
     * Guards current shader
     **/
    class FShaderGuard final {
        GLint LastProgramUsed = 0;

    public:
        FShaderGuard();
        ~FShaderGuard();

        static bool IsInLegacyMode();
    };

    void SetupLegacyGL();

} // Slab::Graphics::OpenGL

#endif //LEGACYMODE_H
