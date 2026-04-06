//
// Created by joao on 8/17/25.
//

#ifndef PARAMETERGUIRENDERER_H
#define PARAMETERGUIRENDERER_H

#include "../../Lib/Core/Controller/Parameter/Parameter.h"


class FParameterGUIRenderer {
public:
    static void RenderParameter(const Slab::TPointer<Slab::Core::FParameter>&);
};

using ParameterGUIRenderer [[deprecated("Use FParameterGUIRenderer")]] = FParameterGUIRenderer;



#endif //PARAMETERGUIRENDERER_H
