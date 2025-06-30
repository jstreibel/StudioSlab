//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_WINDOWTREEBUILDERTEST_H
#define V_SHAPE_WINDOWTREEBUILDERTEST_H


#include "Graphics/Backend/Events/SystemWindowEventListener.h"
#include "Graphics/Window/WindowTree/NodeWindow.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"

class WindowTreeBuilderTest : public Slab::Graphics::FSlabWindow {
    Slab::Graphics::NodeWindow *main;

    Slab::Math::RtoR::AnalyticOscillon osc;
public:
    WindowTreeBuilderTest();
    void ImmediateDraw() override;

    void NotifyReshape(int width, int height) override;

};


#endif //V_SHAPE_WINDOWTREEBUILDERTEST_H
