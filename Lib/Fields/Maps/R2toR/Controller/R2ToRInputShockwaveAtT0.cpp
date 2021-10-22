//
// Created by joao on 23/03/2021.
//

#include <Lib/Fields/Maps/R2toR/View/OutputShockwaveZeros.h>
#include "R2ToRInputShockwaveAtT0.h"
#include "Lib/Fields/Maps/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"
#include "Lib/Fields/Maps/R2toR/Model/FunctionsCollection/R2ToRRegularDelta.h"
#include "Lib/Fields/Maps/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"
#include "Lib/Fields/Maps/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Lib/Fields/Maps/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"
#include "Lib/Fields/Maps/R2toR/Model/R2ToRBoundaryCondition.h"
#include "Lib/Fields/Maps/R2toR/View/R2ToROutputOpenGlShockwaveAtT0.h"


using namespace R2toR;

void R2toRInputShockwaveAt_t0::YoureTheChosenOne(CLVariablesMap &vm) {
    t0 = vm["t0"].as<double>();

    const auto derivative_dt = 1.e-7;
    auto sw = new RtoR::AnalyticShockwave2DRadialSymmetry;
    auto ddtSw = new RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivativeB(derivative_dt);
    sw->sett(t0);
    ddtSw->sett(t0);

    auto *phi0 = new FunctionAzimuthalSymmetry(sw, 1);
    auto *dPhiDt0 = new FunctionAzimuthalSymmetry(ddtSw, 1);

    auto *initCond = new BoundaryCondition(phi0, dPhiDt0);
    dPhi = initCond;

    modelParams.mapping = Allocator::ModelParams::R2toRMap;


    OutputOpenGLShockwaveAt_t0 *outGLShockwave = new OutputOpenGLShockwaveAt_t0(getInterfaceParametersMap());
    outGL = outGLShockwave;

    RtoR2::StraightLine section1, section2;
    {
        const Real rMin = -5;
        const Real rMax =  5;
        const Real2D x0 = {rMin, .0}, xf = {rMax, .0};
        Real theta = 0.0;

        Rotation R;
        R = Rotation(theta);
        section1 = RtoR2::StraightLine(R * x0, R * xf);
        R = Rotation(theta + 1./8 * M_PI);
        section2 = RtoR2::StraightLine(R * x0, R * xf);
    }
    outGLShockwave->addSection(section1, "Section1");// String("theta = ") + std::to_string(theta) + "rad");
    outGLShockwave->addSection(section2, "Section2");// String("theta = ") + std::to_string(theta) + "rad");


    if(vm["outputzeros"].as<bool>()){
        letc N = GET("N", size_t);
        letc outRes = 1024;
        OutputChannel *zerosOut = new R2toR::OutputShockwaveZeros((int)N, (int)(outRes>N?N:outRes), t0);
        myOutputs.push_back(zerosOut);
    }
}

auto R2toRInputShockwaveAt_t0::getParameters() const -> UserParamMap {
    return {{"sw_t0", new DoubleParameter(t0, "t0", "Initial time.")},
            {"output_zeros", new BoolParameter(false, "outputzeros", "Should output to file the zeros of the shockwave?")}};
}

auto R2toRInputShockwaveAt_t0::getGeneralDescription() const -> String {
    return String("2-d shockwave starting at arbitrary t0.");
}

auto R2toRInputShockwaveAt_t0::getOutputs(bool usingOpenGLBackend) const -> std::vector<OutputChannel *> {
    auto outputs = myOutputs;
    if (usingOpenGLBackend) { outputs.push_back(outGL); }

    return outputs;
}
