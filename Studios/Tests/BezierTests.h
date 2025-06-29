//
// Created by joao on 8/09/23.
//

#ifndef STUDIOSLAB_BEZIERTESTS_H
#define STUDIOSLAB_BEZIERTESTS_H

#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Plot2D/Plot2DWindow.h"


namespace Tests {

    using namespace Slab;

    class BezierTests : public Graphics::WindowRow {
        Graphics::FGUIWindow Stats;
        Graphics::Plot2DWindow Graph;

        Math::PointSet CurrentPoint;

    public:
        explicit BezierTests(const Pointer<Graphics::SlabImGuiContext>& GuiContext);

        void Draw() override;
    };

} // Tests

#endif //STUDIOSLAB_BEZIERTESTS_H
