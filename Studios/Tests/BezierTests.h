//
// Created by joao on 8/09/23.
//

#ifndef STUDIOSLAB_BEZIER_TESTS_H
#define STUDIOSLAB_BEZIER_TESTS_H

#include "Graphics/ImGui/ImGui-Multi-SlabWindow-Wrapper.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Window/WindowContainer/WindowRow.h"


namespace Tests {

    using namespace Slab;

    class FBezierTests final : public Graphics::FWindowRow {
        Graphics::FGUIWindow Stats;
        Graphics::FPlot2DWindow Graph;

        Math::PointSet CurrentPoint;

    public:
        explicit FBezierTests(const TPointer<Graphics::FImGuiContext>& GuiContext);

        void ImmediateDraw(const Graphics::FPlatformWindow&) override;

        auto RegisterDeferredDrawCalls(const Graphics::FPlatformWindow& PlatformWindow) -> void override;
    };

} // Tests

#endif //STUDIOSLAB_BEZIER_TESTS_H
