//
// Created by joao on 7/24/24.
//

#ifndef STUDIOSLAB_VSHAPEEXPANSIONTEST_H
#define STUDIOSLAB_VSHAPEEXPANSIONTEST_H

#include "Math/Function/RtoR/Model/FunctionsCollection/NonlinearKGPotential.h"

#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Plot2D/Plot2DWindow.h"

namespace Tests {

    class VShapeExpansionTest : public Slab::Graphics::WindowRow {
        Slab::Graphics::GUIWindow stats;
        Slab::Graphics::Plot2DWindow graph;

        Slab::Pointer<Slab::Math::RtoR::NonlinearKGPotential> potential;
        Slab::Pointer<Slab::Math::Base::FunctionT<Slab::DevFloat, Slab::DevFloat>> derivs;

    public:
        VShapeExpansionTest();

        void draw() override;
    };

} // Tests

#endif //STUDIOSLAB_VSHAPEEXPANSIONTEST_H
