//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_RTOROUTPUTOPENGL_H
#define V_SHAPE_RTOROUTPUTOPENGL_H

#include "Studios/MathTools/Maps/RtoR/Model/RtoRFuncArbResizable.h"
#include "Studios/MathTools/Maps/RtoR/View/EnergyCalculator.h"

#include <Studios/Graphics/GraphCollections.h>
#include <Fields/View/Graphic/OutputOpenGL.h>


namespace RtoR {

    class OutputOpenGL : public Base::OutputOpenGL {
        bool isInitialized = false;
    public:
        OutputOpenGL(double xMin, double xMax, double phiMin, double phiMax);

        OutputOpenGL() = default;

        void initialize(double xMin, double xMax, double phiMin, double phiMax);

    protected:
        void draw() override;
        void _out(const OutputPacket &outInfo) override;

    private:
        void notifyGLUTKeyboard(unsigned char key, int x, int y) override;
        void notifyGLUTKeyboardSpecial(int key, int x, int y) override;
        void notifyGLUTMouseButton(int button, int dir, int x, int y) override;

    public:
        void reshape(int width, int height) override;

    protected:
        auto getPhiMax() const -> double { return fieldsGraph.yMax; }
        auto getPhiMin() const -> double { return fieldsGraph.yMin; }

        void setPhiMax(const double newPhiMax) {
            phiMaxAnim->animateToValue(newPhiMax);
        }

        void setPhiMin(const double newPhiMin) {
            phiMinAnim->animateToValue(newPhiMin);
        }

        Animation *phiMinAnim;
        Animation *phiMaxAnim;
        Animation *xMinAnim;
        Animation *xMaxAnim;

        Graph fieldsGraph;

        Graph totalEnergyGraph;
        Graph phaseSpaceGraph;
        EnergyCalculator energyCalculator;
        FuncArbResizable energyHistory;
        Real energyTotal;

        bool showPhi = true;
        bool showDPhiDt = false;
        bool showEnergyDensity = false;



    protected:
        double xMin, xMax;

        double phiMax;
        double phiMin;
        bool showAnalyticSolution = false;
    };

}


#endif //V_SHAPE_RTOROUTPUTOPENGL_H
