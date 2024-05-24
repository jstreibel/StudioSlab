//
// Created by joao on 4/09/23.
//

#ifndef STUDIOSLAB_FOURIERTESTWINDOW_H
#define STUDIOSLAB_FOURIERTESTWINDOW_H

#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "Graphics/Window/WindowContainer/WindowColumn.h"
#include "Graphics/Graph/RtoR/GraphRtoR.h"

namespace Tests {

class FourierTestWindow : public Graphics::Window {
        Graphics::WindowRow row;
        Graphics::WindowColumn col;

        Graphics::GraphRtoR mFuncGraph;
        Graphics::PlottingWindow mDFTGraph;
        Graphics::GraphRtoR mFTGraph;
        Graphics::GUIWindow gui;

        void updateGraphs();
    public:
        FourierTestWindow();

        void draw() override;

        void notifyReshape(int w, int h) override;

        bool notifyMouseMotion(int x, int y) override;

        bool notifyMouseWheel(double dx, double dy) override;
    };

} // Tests

#endif //STUDIOSLAB_FOURIERTESTWINDOW_H
