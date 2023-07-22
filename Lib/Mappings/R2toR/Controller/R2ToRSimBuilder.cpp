//
// Created by joao on 7/19/23.
//

#include "R2ToRSimBuilder.h"

#include "Base/Backend/GLUT/GLUTBackend.h"
#include "Base/Backend/Console/ConsoleBackend.h"

#include "Mappings/R2toR/Model/R2toRDiscreteFunctionCPU.h"
#include "Mappings/R2toR/Model/R2toRDiscreteFunctionGPU.h"
#include "Mappings/R2toR/Model/EquationState.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AbsFunction.h"

#include "Phys/DifferentialEquations/2nd-Order/GordonSystemT.h"
#include "Phys/Numerics/Output/Format/OutputFormatterBase.h"
#include "Phys/Numerics/Output/Format/BinarySOF.h"
#include "Phys/Numerics/Output/Format/SpaceFilterBase.h"
#include "Mappings/R2toR/View/Filters/DimensionReductionFilter.h"
#include "Phys/Numerics/Output/Plugs/OutputHistoryToFile.h"
#include "Phys/Numerics/Output/Plugs/OutputConsoleMonitor.h"

R2toR::Simulation::GordonBuilder::GordonBuilder(Str name, Str description)
: Base::Simulation::Builder<R2toR::EquationSolver>(name, description) {

}

OutputManager *R2toR::Simulation::GordonBuilder::buildOutputManager() {
    auto outputFileName = this->toString();

    const auto shouldOutputOpenGL = *VisualMonitor;
    const auto shouldTrackHistory = ! *noHistoryToFile;


    if(*VisualMonitor) Backend::Initialize<GLUTBackend>();
    else               Backend::Initialize<ConsoleBackend>();

    auto *outputManager = new OutputManager(numericParams);

    RtoR2::StraightLine section1, section2;
    {
        const Real sqrt2 = sqrt(2.);
        const Real rMin = numericParams.getxLeft();
        const Real rMax = numericParams.getxMax();
        const Real2D x0 = {rMin, .0}, xf = {rMax, .0};

        Real theta = 0.0;

        Rotation R;
        R = Rotation(theta);
        section1 = RtoR2::StraightLine(R * x0, R * xf, rMin, rMax);
        R = Rotation(theta + .5 * M_PI);
        section2 = RtoR2::StraightLine(R * x0, R * xf);
    }


    ///********************************************************************************************/
    int fileOutputStepsInterval = -1;
    if(shouldTrackHistory)
    {
        const Real t=numericParams.gett();
        const PosInt outputResolutionX = *outputResolution;

        OutputFormatterBase *outputFilter = new BinarySOF;

        SpaceFilterBase *spaceFilter = new DimensionReductionFilter(
                numericParams.getN(), section1);

        const auto N = (Real) numericParams.getN();
        const Real Np = outputResolutionX;
        const Real r = numericParams.getr();
        const auto stepsInterval = PosInt(N/(Np*r));

        auto fileName = outputFileName + "-N=" + ToStr(N, 0);

        Numerics::OutputSystem::Socket *out = new OutputHistoryToFile(numericParams, stepsInterval, spaceFilter, t, fileName, outputFilter);

        fileOutputStepsInterval = out->getnSteps();
        outputManager->addOutputChannel(out);
    }
    ///********************************************************************************************/


    ///********************************************************************************************/
    if(shouldOutputOpenGL) {
        GUIBackend &backend = GUIBackend::GetInstance(); // GLUTBackend precisa ser instanciado, de preferencia, antes dos OutputOpenGL.
        if((*VisualMonitor_startPaused)) backend.pause();
        else backend.resume();

        auto glOut = Graphics::OutputOpenGL::Ptr(this->buildOpenGLOutput());
        glOut->setnSteps(*OpenGLMonitor_stepsPerIdleCall);

        backend.addWindow(glOut);
        outputManager->addOutputChannel(glOut.get(), false);
    }
    else {
        /* O objetivo de relacionar o numero de passos para o Console Monitor com o do file output eh para que
         * ambos possam ficar sincronizados e o integrador possa rodar diversos passos antes de fazer o output. */
        outputManager->addOutputChannel(
                new OutputConsoleMonitor(numericParams, fileOutputStepsInterval > 0
                                                        ? fileOutputStepsInterval * 25
                                                        : int(numericParams.getn() / 40)));
    }

    return outputManager;

}

void *R2toR::Simulation::GordonBuilder::newFunctionArbitrary() {
    const size_t N = numericParams.getN();
    const floatt xLeft = numericParams.getxLeft();

    if(dev==CPU)
        return new R2toR::FunctionArbitraryCPU(N, N, xLeft, xLeft, numericParams.geth());

#if USE_CUDA
    else if(dev==GPU)
        return new R2toR::FunctionArbitraryGPU(N, xLeft, numericParams.geth());
#endif

    throw "Error while instantiating Field: device not recognized.";
}

void *R2toR::Simulation::GordonBuilder::getInitialState() {
    return new R2toR::EquationState((R2toR::DiscreteFunction*)this->newFunctionArbitrary(),
                                    (R2toR::DiscreteFunction*)this->newFunctionArbitrary());
}

void *R2toR::Simulation::GordonBuilder::getEquationSolver() {
    RtoR::Function *thePotential = new RtoR::AbsFunction;

    // return new Phys::Gordon::GordonSystem<R2toR::EquationState>(*this, *thePotential);
    auto solver = new Phys::Gordon::GordonSolverT<EqState>(*getInitialState(), *thePotential);

    return
}

auto R2toR::Simulation::Builder::buildOpenGLOutput() -> R2toR::OutputOpenGL * {
    return new R2toR::OutputOpenGL(numericParams, -1, 1);
}


