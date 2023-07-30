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
#include "Phys/DifferentialEquations/2nd-Order/GordonSystem.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Mappings/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"
#include "Mappings/R2toR/View/LastOutputVtkVisualizer.h"

namespace R2toR {

    Builder::Builder(Str name, Str description)
            : Base::Simulation::VoidBuilder(name, description) {
    }

    OutputManager *Builder::buildOutputManager() {
        const auto shouldOutputOpenGL = *VisualMonitor;
        const auto shouldTrackHistory = !*noHistoryToFile;

        if (*VisualMonitor) Backend::Initialize<GLUTBackend>();
        else Backend::Initialize<ConsoleBackend>();

        auto *outputManager = new OutputManager(numericParams);

        outputManager->addOutputChannel(new LastOutputVTKVisualizer(numericParams, numericParams.getN()));

        RtoR2::StraightLine section;
        auto angleDegrees = 22.5;
        {
            const Real rMin = numericParams.getxLeft();
            const Real rMax = numericParams.getxMax();
            const Real2D x0 = {rMin, .0}, xf = {rMax, .0};

            Rotation R;
            R = Rotation(M_PI*angleDegrees/180);
            section = RtoR2::StraightLine(R * x0, R * xf);
        }


        ///********************************************************************************************/
        int fileOutputStepsInterval = -1;
        if (shouldTrackHistory) {
            const Real t = numericParams.gett();
            const PosInt outputResolutionX = *outputResolution;

            OutputFormatterBase *outputFilter = new BinarySOF;

            SpaceFilterBase *spaceFilter = new DimensionReductionFilter(
                    outputResolutionX, section);

            const auto N = (Real) numericParams.getN();
            const Real Np = outputResolutionX;
            const Real r = numericParams.getr();
            const auto stepsInterval = PosInt(N / (Np * r));

            auto outputFileName = this->buildFileName() + " section_tx_angle=" + ToStr(angleDegrees, 1);

            Numerics::OutputSystem::Socket *out = new OutputHistoryToFile(numericParams, stepsInterval,
                                                                          spaceFilter, t, outputFileName,
                                                                          outputFilter);

            fileOutputStepsInterval = out->getnSteps();
            outputManager->addOutputChannel(out);
        }
        ///********************************************************************************************/


        ///********************************************************************************************/
        if (shouldOutputOpenGL) {
            GUIBackend &backend = GUIBackend::GetInstance(); // GLUTBackend precisa ser instanciado, de preferencia, antes dos OutputOpenGL.
            if ((*VisualMonitor_startPaused)) backend.pause();
            else backend.resume();

            auto glOut = Graphics::OutputOpenGL::Ptr(this->buildOpenGLOutput());
            glOut->setnSteps(*OpenGLMonitor_stepsPerIdleCall);

            backend.addWindow(glOut);
            outputManager->addOutputChannel(glOut.get(), false);
        } else {
            /* O objetivo de relacionar o numero de passos para o Console Monitor com o do file output eh para que
             * ambos possam ficar sincronizados e o integrador possa rodar diversos passos antes de fazer o output. */
            outputManager->addOutputChannel(
                    new OutputConsoleMonitor(numericParams, fileOutputStepsInterval > 0
                                                            ? fileOutputStepsInterval * 25
                                                            : int(numericParams.getn() / 40)));
        }

        return outputManager;

    }

    void *Builder::newFunctionArbitrary() {
        const size_t N = numericParams.getN();
        const floatt xLeft = numericParams.getxLeft();

        if (dev == CPU)
            return new R2toR::FunctionArbitraryCPU(N, N, xLeft, xLeft, numericParams.geth());

#if USE_CUDA
        else if (dev == GPU)
            return new R2toR::FunctionArbitraryGPU(N, xLeft, numericParams.geth());
#endif

        throw "Error while instantiating Field: device not recognized.";
    }

    void *Builder::getEquationSolver() {
        auto thePotential = new RtoR::AbsFunction;
        auto dphi = (R2toR::BoundaryCondition*)getBoundary();

        return new Phys::Gordon::GordonSolverT<R2toR::EquationState>(numericParams, *dphi, *thePotential);
    }

    auto Builder::buildOpenGLOutput() -> R2toR::OutputOpenGL * {
        return new R2toR::OutputOpenGL(numericParams, -1, 1);
    }

    auto Builder::newFieldState() -> void * {
        auto u   = (R2toR::DiscreteFunction*)newFunctionArbitrary();
        auto du  = (R2toR::DiscreteFunction*)newFunctionArbitrary();

        return new R2toR::EquationState(u, du);
    }

#define GENERATE_METHOD(METHOD, N) \
    case (N): \
    method = new METHOD<N, typename R2toR::EquationState>(solver); \
    break;

#define GENERATE_ALL(METHOD) \
        GENERATE_METHOD(StepperRK4, 1);  \
        GENERATE_METHOD(StepperRK4, 2);  \
        GENERATE_METHOD(StepperRK4, 3);  \
        GENERATE_METHOD(StepperRK4, 4);  \
        GENERATE_METHOD(StepperRK4, 5);  \
        GENERATE_METHOD(StepperRK4, 6);  \
        GENERATE_METHOD(StepperRK4, 7);  \
        GENERATE_METHOD(StepperRK4, 8);  \
        GENERATE_METHOD(StepperRK4, 9);  \
        GENERATE_METHOD(StepperRK4, 10); \
        GENERATE_METHOD(StepperRK4, 11); \
        GENERATE_METHOD(StepperRK4, 12); \
        GENERATE_METHOD(StepperRK4, 13); \
        GENERATE_METHOD(StepperRK4, 14); \
        GENERATE_METHOD(StepperRK4, 15); \
        GENERATE_METHOD(StepperRK4, 16);

    Method *Builder::buildStepper() {
        Method *method;

        auto &solver = *(R2toR::EquationSolver*)getEquationSolver();

        switch (dev.get_nThreads()) {
            GENERATE_ALL(StepperRK4);
            default:
                throw "Number of threads must be between 1 and 16 inclusive.";
        }

        return method;
    }

    void *Builder::getInitialState() {
        RtoR::NullFunction nullFunction;
        R2toR::FunctionAzimuthalSymmetry fullNull(&nullFunction, 1, 0, 0, false);

        auto &u_0 = *(R2toR::EquationState*)newFieldState();

        u_0.setPhi(fullNull);
        u_0.setDPhiDt(fullNull);

        return &u_0;
    }
};