//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_RTOROUTPUTOPENGL_H
#define V_SHAPE_RTOROUTPUTOPENGL_H

#include "Mappings/RtoR/Calc/EnergyCalculator.h"
#include "Mappings/RtoR/Model/RtoRFuncArbResizable.h"
#include "Base/Graphics/Window/WindowContainer/WindowPanel.h"

#include "Mappings/RtoR/View/Graphics/Artists/GraphRtoR.h"
#include "Phys/Graph/OutputOpenGL.h"

namespace RtoR {

    class Monitor : public Graphics::OutputOpenGL {
        bool isInitialized = false;
    public:
        Monitor(Real xMin, Real xMax, Real phiMin, Real phiMax);
        Monitor();

        void initialize(Real xMin, Real xMax, Real phiMin, Real phiMax);

    protected:
        void draw() override;
        void _out(const OutputPacket &outInfo"") override;

    private:
        bool notifyKeyboard(unsigned char key, int x, int y) override;

    protected:
        GraphRtoR mFieldsGraph;

        //GraphRtoR mTotalEnergyGraph;

        EnergyCalculator energyCalculator;
        //FuncArbResizable UHistory;
        //FuncArbResizable KHistory;
        //FuncArbResizable WHistory;
        //FuncArbResizable VHistory;
        //Real energyTotal;

        const Styles::Color U_color = {.9f, .5f, .0f, .9};
        const Styles::Color K_color = {1,0,0};
        const Styles::Color W_color = {0.5,1,0.5};
        const Styles::Color V_color = {.5f, .5f, 1.0f};

        bool showPhi = true;
        bool showKineticEnergy = false;
        bool showGradientEnergy = false;
        bool showEnergyDensity = false;

        //bool showAnalyticSolution = false;
    };

}


#endif //V_SHAPE_RTOROUTPUTOPENGL_H
