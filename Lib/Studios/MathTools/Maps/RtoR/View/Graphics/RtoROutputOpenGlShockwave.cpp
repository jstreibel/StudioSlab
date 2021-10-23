//
// Created by joao on 15/07/2021.
//

#include "RtoROutputOpenGlShockwave.h"

#include <Studios/Graphics/FunctionRenderer.h>

#include <Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/AnalyticShockwave1D.h>
#include <Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/QuasiShockwave.h>

#include <Studios/Backend/Graphic/GLUTBackend.h>

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)

RtoR::OutputOpenGLShockwave::OutputOpenGLShockwave(double a0, double E) : a0(a0), E(E) {
    surfaceEnergyHistory.xMin = 0;
    surfaceEnergyHistory.xMax = Allocator::getInstance().getNumericParams().gett();

    innerEnergyHistory.xMin = 0;
    innerEnergyHistory.xMax = Allocator::getInstance().getNumericParams().gett();

}

void RtoR::OutputOpenGLShockwave::draw() {
    OutputOpenGL::draw();

    if(showAnalyticSolution){
        const Real t = lastInfo.getT();

        //AnalyticShockwave1D shockwave1D(a0);
        //shockwave1D.setT(t);
        //RtoR::FunctionRenderer::renderFunction(shockwave1D, Color(.0, 1, 1, .3), true, xMin, xMax, 2000);

        QuasiShockwave quasiShockwave(a0, E, t);
        RtoR::FunctionRenderer::renderFunction(quasiShockwave, Color(.0, .0, 1., .8), true, xMin, xMax, 2000);
    }

    {
        const auto eps = 4*a0*a0 / (3*E);
        const auto v = -1 + eps*eps/(2*a0);

        const auto t = getLastT();


        {
            glColor4f(.2, .4, .8, .85);

            glBegin(GL_LINES);
            glVertex2d(eps - t, getPhiMin());
            glVertex2d(eps - t, getPhiMax());
            glVertex2d(-eps + v * t, getPhiMin());
            glVertex2d(-eps + v * t, getPhiMax());

            glVertex2d(-eps + t, getPhiMin());
            glVertex2d(-eps + t, getPhiMax());
            glVertex2d(eps - v * t, getPhiMin());
            glVertex2d(eps - v * t, getPhiMax());
            glEnd();


        }


        auto surfaceEnergy = .0;
        auto innerEnergy = .0;
        if (t < eps) {
            surfaceEnergy = energyCalculator.integrate(-eps + v * t, eps - v * t);
            innerEnergy = 0;
        } else {
            surfaceEnergy = energyCalculator.integrate(-eps + v * t, eps - t) +
                            energyCalculator.integrate(-eps + t, eps - v * t);
            innerEnergy = energyCalculator.integrate(eps + v * t, -eps - v * t);;
        }

        if(GLUTBackend::GetInstance()->isRunning())
        {
            surfaceEnergyHistory.insertBack(surfaceEnergy);
            surfaceEnergyHistory.xMax = totalEnergyGraph.xMax;
            innerEnergyHistory.insertBack(innerEnergy);
            innerEnergyHistory.xMax = totalEnergyGraph.xMax;

            totalEnergyGraph.yMax = max(energyHistory.getYMax(), surfaceEnergyHistory.getYMax());
            totalEnergyGraph.yMin = min(innerEnergyHistory.getYMin(), surfaceEnergyHistory.getYMax());
        }

        addVolatileStat(std::string("E_surf=") + std::to_string(surfaceEnergy));
        addVolatileStat(std::string("E_in=") + std::to_string(innerEnergy));
        addVolatileStat(std::string("E_tot=E_surf+E_in=") + std::to_string(surfaceEnergy+innerEnergy));

        addVolatileStat(std::string("a=") + std::to_string(2*a0));
        addVolatileStat(std::string("eps=") + std::to_string(eps));
        addVolatileStat(std::string("t_c=a/eps=") + std::to_string(2*a0/eps));
    }

    totalEnergyGraph.BindWindow(false);
    FunctionRenderer::renderFunction(surfaceEnergyHistory, Color(.9f, .5f, .0f, .9),
                                     false, energyHistory.xMin, energyHistory.xMax, surfaceEnergyHistory.X.size());
    FunctionRenderer::renderFunction(innerEnergyHistory, Color(.9f, .5f, .0f, .9),
                                     false, energyHistory.xMin, energyHistory.xMax, innerEnergyHistory.X.size());
}

