//
// Created by joao on 4/09/23.
//

#ifndef STUDIOSLAB_FOURIERTESTWINDOW_H
#define STUDIOSLAB_FOURIERTESTWINDOW_H

#include "Maps/RtoR/View/Graphics/GraphRtoR.h"
#include "Core/Graphics/Window/GUIWindow.h"
#include "Core/Graphics/Window/WindowContainer/WindowRow.h"
#include "Core/Graphics/Window/WindowContainer/WindowColumn.h"

namespace Tests {

    class FourierTestWindow : public Window {
        WindowRow row;
        WindowColumn col;


        GraphRtoR mFuncGraph;
        Core::Graphics::Graph2D mDFTGraph;
        GUIWindow gui;

        void updateGraphs();
    public:
        FourierTestWindow();

        void draw() override;


        void notifyReshape(int w, int h) override;
    };

} // Tests

#endif //STUDIOSLAB_FOURIERTESTWINDOW_H
