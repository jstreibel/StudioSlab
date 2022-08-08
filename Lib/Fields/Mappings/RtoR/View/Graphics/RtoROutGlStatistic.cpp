//
// Created by joao on 7/12/22.
//

#include "RtoROutGlStatistic.h"

#include "../../../FunctionRenderer.h"
#include "Fields/Mappings/RtoR/Model/RtoRFunctionArbitraryCPU.h"

#include "Fields/Mappings/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"
#include "Fields/Mappings/RtoR/Model/Derivatives/DerivativesCPU.h"

#include <sstream>

RtoR::OutGLStatistic::OutGLStatistic() {
    const Real xLeft = Allocator::getInstance().getNumericParams().getxLeft();
    const Real xRight = xLeft + Allocator::getInstance().getNumericParams().getL();

    initialize(xLeft, xRight, -0.08, 0.08);
}

void RtoR::OutGLStatistic::draw() {
    RtoR::OutputOpenGL::draw();

    const RtoR::FieldState &fieldState = *lastInfo.getFieldData<RtoR::FieldState>();
    if(&fieldState == nullptr) throw "Fieldstate data doesn't seem to be RtoRMap.";


    // *************************** PHASE SPACE *****************************
    phaseSpaceGraph.DrawAxes();

    auto &Q = fieldState.getPhi().getSpace().getX(),
            &P = fieldState.getDPhiDt().getSpace().getX();
    auto N = (double)Q.size();

    #define sign(x) ((x)<0?-1.:1.)
    auto rq = .0, rp = .0;
    for(auto &q : Q) rq+=q;
    for(auto &p : P) rp+=p;
    rq /= N;
    rp /= N;

    static std::vector<std::pair<double, double>> points;
    points.emplace_back(rq, rp);

    glPointSize(2);
    glColor4f(1,1,1,0.4f);
    glBegin(GL_POINTS);
    for(auto &point : points) glVertex2f(point.first, point.second);
    glEnd();




    // *************************** MY BEAUTY *****************************
    auto h = Allocator::getInstance().getNumericParams().geth();
    auto L = Allocator::getInstance().getNumericParams().getL();

    auto u = energyTotal/L;
    auto barϕ = energyCalculator.integratePotential() / L;
    auto K = energyCalculator.integrateKinetic();
    auto W = 2*energyCalculator.integrateGradient();

    auto tau = 2*K/L;
    auto tau_indirect = u - .5*barϕ;

    std::ostringstream ss;

    addVolatileStat(String("N = ") + std::to_string((int)N));
    addVolatileStat(String("L = ") + std::to_string(L));
    addVolatileStat(String("L/N = ") + std::to_string(L/N));
    addVolatileStat(String("h = ") + std::to_string(h));
    addVolatileStat(String(""));

    ss.str(""); ss << "U = " << energyTotal; addVolatileStat(ss.str());
    ss.str(""); ss << "u = U/L = " << u; addVolatileStat(ss.str());
    ss.str(""); ss << "barphi = " << barϕ; addVolatileStat(ss.str());
    ss.str(""); ss << "w = " << W/L; addVolatileStat(ss.str());
    ss.str(""); ss << "barphi + w = tau = " << (barϕ+W/L); addVolatileStat(ss.str());
    ss.str(""); ss << "tau = <dotphi^2> = 2K/L = " << tau; addVolatileStat(ss.str());
    ss.str(""); ss << "tau* = u - barphi/2 = " << tau_indirect; addVolatileStat(ss.str());





    // *************************** ENERGY *********************************
    totalEnergyGraph.DrawAxes();
    FunctionRenderer::renderFunction(energyHistory, Color(.9f, .5f, .0f, .9),
                                     false, energyHistory.xMin, energyHistory.xMax, energyHistory.X.size());

    addVolatileStat(std::string("E: ") + std::to_string(energyTotal));
    addVolatileStat(std::string("E.size(): ") + std::to_string(energyHistory.X.size()));
}
