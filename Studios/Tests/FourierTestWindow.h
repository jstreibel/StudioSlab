//
// Created by joao on 4/09/23.
//

#ifndef STUDIOSLAB_FOURIERTESTWINDOW_H
#define STUDIOSLAB_FOURIERTESTWINDOW_H

#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "Graphics/Window/WindowContainer/WindowColumn.h"
#include "Graphics/Plot2D/Artists/PointSetArtist.h"
#include "Graphics/Plot2D/Artists/RtoRFunctionArtist.h"
#include "Graphics/Plot2D/PlottingTheme.h"

namespace Tests {

    using namespace Slab;

    class FourierTestWindow : public Graphics::SlabWindow {
        Pointer<Graphics::PlottingTheme> theme;

        Graphics::WindowRow row;
        Graphics::WindowColumn col;

        Graphics::Plot2DWindow mFuncGraph;
        Graphics::Plot2DWindow mDFTGraph;
        Graphics::Plot2DWindow mFTGraph;
        Graphics::GUIWindow gui;

        Graphics::PointSetArtist realFTArtist;
        Graphics::PointSetArtist imagFTArtist;
        Graphics::PointSetArtist loc1Artist, loc2Artist;
        Graphics::RtoRFunctionArtist funcArtist;
        Graphics::RtoRFunctionArtist rebuiltFuncArtist;


        void updateGraphs();
    public:
        FourierTestWindow();

        void draw() override;

        void notifyReshape(int w, int h) override;

        bool notifyMouseMotion(int x, int y, int dx, int dy) override;

        bool notifyMouseWheel(double dx, double dy) override;
    };

} // Tests

#endif //STUDIOSLAB_FOURIERTESTWINDOW_H
