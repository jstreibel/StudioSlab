//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_WINDOWPANELTEST_H
#define V_SHAPE_WINDOWPANELTEST_H


#include <Graphics/Backend/Events/SystemWindowEventListener.h>
#include "Graphics/Window/SlabWindow.h"
#include "Graphics/Window/WindowContainer/WindowPanel.h"


class FWindowPanelTest : public Slab::Graphics::FSlabWindow {
    Slab::Graphics::WindowPanel *panel;

public:
    FWindowPanelTest();
    void ImmediateDraw(const Slab::Graphics::FPlatformWindow&) override;

    void NotifyReshape(int width, int height) override;

};

using WindowPanelTest [[deprecated("Use FWindowPanelTest")]] = FWindowPanelTest;


#endif //V_SHAPE_WINDOWPANELTEST_H
