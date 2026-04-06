//
// Created by joao on 29/09/23.
//

#ifndef STUDIOSLAB_MODERNOPENGLMODULE_H
#define STUDIOSLAB_MODERNOPENGLMODULE_H

#include "Core/Backend/Modules/Module.h"

namespace Slab::Graphics {

    class FModernOpenGLModule : public Core::FSlabModule {
    public:
        FModernOpenGLModule();
    };

    using ModernOpenGLModule [[deprecated("Use FModernOpenGLModule")]] = FModernOpenGLModule;

} // Core

#endif //STUDIOSLAB_MODERNOPENGLMODULE_H
