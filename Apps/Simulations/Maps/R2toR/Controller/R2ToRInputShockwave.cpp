//
// Created by joao on 23/09/2019.
//

#include "R2ToRInputShockwave.h"
#include "Apps/Simulations/Maps/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"
#include "Apps/Simulations/Maps/R2toR/Model/FunctionsCollection/R2ToRRegularDelta.h"
#include "Apps/Simulations/Maps/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"
#include "Apps/Simulations/Maps/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Apps/Simulations/Maps/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"
#include "Apps/Simulations/Maps/R2toR/Model/R2ToRBoundaryCondition.h"
#include "Apps/Simulations/Maps/R2toR/View/R2toROutputOpenGLShockwave.h"
#include "Apps/Simulations/Maps/R2toR/View/OutputShockwaveZeros.h"


using namespace R2toR;

auto R2toRInputShockwave::getBoundary() const -> const void * {
    E = vm["E"].as<double>();
    eps = vm["epsilon"].as<double>();
    const double a = sqrt((4./3)*pi*eps*eps*E);

    let *phi0 = new FunctionAzimuthalSymmetry(new RtoR::NullFunction(*new RtoR::NullFunction), e, theta);
    //auto *dPhiDt0 = new FunctionAzimuthalSymmetry(new RtoRMap::RegularDiracDelta(a, a), sqrt(3./M_PI),e, theta);
    let *dPhiDt0 = new R2toR::R2toRRegularDelta(eps, a);

    let *initCond = new BoundaryCondition(phi0, dPhiDt0);
    dPhi = initCond;

    if(0) // Output da energia nominal.
    {
        {
            Real E = 0.0;
            Real N = 4096;
            const Real h = 2. * eps / N;
            for (Real x = -eps; x <= eps; x += h) {
                for (Real y = -eps; y <= eps; y += h) {
                    Real2D r = {x, y};
                    const Real val = (*dPhiDt0)(r);
                    E += val * val;
                }
            }
            E *= .5 * h * h;
            std::cout << "\nCartesianas: E = " << E << std::endl;
        }
    }

    modelParams.mapping = Allocator::ModelParams::R2toRMap;

    auto *outGLShockwave = new OutputOpenGLShockwave(getParameters());
    outGL = outGLShockwave;

    RtoR2::StraightLine section1, section2;
    {
        //const Real rMin = ModelBuilder::getInstance().getParams().getxLeft();
        //const Real rMax = rMin + ModelBuilder::getInstance().getParams().getL();
        const Real rMin = -5;
        const Real rMax =  5;
        const Real2D x0 = {rMin, .0}, xf = {rMax, .0};
        Real theta = 0.0;

        Rotation R;
        R = Rotation(theta + .5 * M_PI);
        section1 = RtoR2::StraightLine(R * x0, R * xf);
        R = Rotation(theta);
        section2 = RtoR2::StraightLine(R * x0, R * xf);
    }
    outGLShockwave->addSection(section1, "Section1");// String("theta = ") + std::to_string(theta) + "rad");
    outGLShockwave->addSection(section2, "Section2");// String("theta = ") + std::to_string(theta) + "rad");

    if(vm["outputzeros"].as<bool>()){
        letc N = GET("N", size_t);
        letc outRes = 1024;
        OutputChannel *zerosOut = new R2toR::OutputShockwaveZeros((int)N, (int)(outRes>N?N:outRes));
        myOutputs.push_back(zerosOut);
    }
}

auto R2toRInputShockwave::getParameters() const -> UserParamMap {
    return {{"sw_E",     new DoubleParameter(E, "E", "Shockwave simulation total energy parameter.")},
            {"sw_eps",   new DoubleParameter(eps, "epsilon", "Shockwave simulation delta \'width\' parameter.")},
            {"output_zeros", new BoolParameter(false, "outputzeros", "Should output to file the zeros of the shockwave?")}};
}

auto R2toRInputShockwave::getGeneralDescription() const -> String {
    return String("2-d shockwave starting at t=0.");
}

auto R2toRInputShockwave::getOutputs(bool usingOpenGLBackend) const -> std::vector<OutputChannel *> {
    auto outputs = myOutputs;
    if (usingOpenGLBackend) { outputs.push_back(outGL); }

    return outputs;
}

