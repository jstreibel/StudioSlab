//
// Created by joao on 4/10/23.
//

#include "Graph3DTests.h"
#include "Graphics/Plot3D/Scene3DWindow.h"
#include "Graphics/Plot3D/Actors/TestActor.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Graphics/Plot3D/Actors/R2toRFunctionActor.h"

namespace Tests {
    Graph3DTests::Graph3DTests() : Slab::Graphics::WindowRow("Plot3D test") {
        using namespace Slab;

        auto graph3d = New<Graphics::Scene3DWindow>();

        addWindow(graph3d);

        fix N = 5;
        fix M = 5;
        fix xMin = -5.;
        fix yMin = -5.;
        fix w = 10.;
        fix h = 10.;

        auto funky = New<Math::R2toR::NumericFunction_CPU>(N, M, xMin, yMin, w/N, h/M);

        for(auto i=0; i<funky->getN(); ++i)
            for(auto j=0; j<funky->getM(); ++j) {
                fix s = (Real)i/(funky->getN()-1);
                fix t = (Real)j/(funky->getM()-1);

                fix x = s*funky->getDomain().getLx() + xMin;
                fix y = t*funky->getDomain().getLy() + yMin;

                funky->At(i, j) = exp(-(x*x + y*y));
            }

        auto actor = New<Graphics::R2toRFunctionActor>(funky);

        graph3d->addActor(actor);
    }

    void Graph3DTests::draw() {
        WindowRow::draw();
    }
} // Tests