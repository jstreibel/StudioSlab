//
// Created by joao on 7/19/23.
//

#include "R2toRBuilder.h"

#include <utility>

#include "Core/Backend/BackendManager.h"

#include "Math/Function/R2toR/Model/R2toRDiscreteFunctionCPU.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunctionGPU.h"
#include "EquationState.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h"

#include "Models/KleinGordon/KGSolver.h"
#include "Math/Numerics/Output/Format/OutputFormatterBase.h"
#include "Math/Numerics/Output/Format/BinarySOF.h"
#include "Math/Numerics/Output/Format/SpaceFilterBase.h"
#include "Math/Numerics/Output/Plugs/OutputHistoryToFile.h"
#include "Math/Numerics/Output/Plugs/OutputConsoleMonitor.h"

#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/R2toR/Output/Filters/DimensionReductionFilter.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"

namespace R2toR {

    Builder::Builder(const Str& name, Str description)
            : Fields::KleinGordon::KGBuilder(name, std::move(description)) {    }

    OutputManager *Builder::buildOutputManager() {
        const auto shouldOutputOpenGL = *VisualMonitor;
        const auto shouldTrackHistory = !*noHistoryToFile;

        if (*VisualMonitor) Core::BackendManager::Startup(Core::GLFW);
        else                Core::BackendManager::Startup(Core::Headless);

        auto *outputManager = new OutputManager(simulationConfig.numericConfig);

        // outputManager->addOutputChannel(new LastOutputVTKVisualizer(numericParams, numericParams.getN()));

        RtoR2::StraightLine section;
        auto angleDegrees = 22.5;
        {
            const Real rMin = simulationConfig.numericConfig.getxMin();
            const Real rMax = simulationConfig.numericConfig.getxMax();
            const Real2D x0 = {rMin, .0}, xf = {rMax, .0};

            Rotation R;
            R = Rotation(M_PI*angleDegrees/180);
            section = RtoR2::StraightLine(R * x0, R * xf);
        }


        ///********************************************************************************************/
        int fileOutputStepsInterval = -1;
        if (shouldTrackHistory) {
            const Real t = simulationConfig.numericConfig.gett();
            const UInt outputResolutionX = *outputResolution;

            OutputFormatterBase *outputFilter = new BinarySOF;

            SpaceFilterBase *spaceFilter = new DimensionReductionFilter(
                    outputResolutionX, section);

            const auto N = (Real) simulationConfig.numericConfig.getN();
            const Real Np = outputResolutionX;
            const Real r = simulationConfig.numericConfig.getr();
            const auto stepsInterval = UInt(N / (Np * r));

            auto outputFileName = this->suggestFileName() + " section_tx_angle=" + ToStr(angleDegrees, 1);

            Numerics::OutputSystem::Socket *out = new OutputHistoryToFile(simulationConfig.numericConfig, stepsInterval,
                                                                          spaceFilter, t, outputFileName,
                                                                          outputFilter);

            fileOutputStepsInterval = out->getnSteps();
            outputManager->addOutputChannel(out);
        }
        ///********************************************************************************************/


        ///********************************************************************************************/
        if (shouldOutputOpenGL) {
            GraphicBackend &backend = Core::BackendManager::GetGUIBackend();
            if ((*VisualMonitor_startPaused)) backend.pause();
            else backend.resume();

            auto glOut = ::Graphics::OpenGLMonitor::Ptr(this->buildOpenGLOutput());
            glOut->setnSteps(*OpenGLMonitor_stepsPerIdleCall);

            backend.addEventListener(glOut);
            outputManager->addOutputChannel(glOut.get(), false);
        } else {
            /* O objetivo de relacionar o numero de passos para o Console Monitor com o do file output eh para que
             * ambos possam ficar sincronizados e o integrador possa rodar diversos passos antes de fazer o output. */
            outputManager->addOutputChannel(
                    new OutputConsoleMonitor(simulationConfig.numericConfig, fileOutputStepsInterval > 0
                                                            ? fileOutputStepsInterval * 25
                                                            : int(simulationConfig.numericConfig.getn() / 40)));
        }

        return outputManager;

    }

    void *Builder::newFunctionArbitrary() {
        const size_t N = simulationConfig.numericConfig.getN();
        const floatt xLeft = simulationConfig.numericConfig.getxMin();
        fix h = simulationConfig.numericConfig.geth();

        if (simulationConfig.dev == CPU)
            return new R2toR::DiscreteFunction_CPU(N, N, xLeft, xLeft, h, h);

#if USE_CUDA
        else if (simulationConfig.dev == GPU)
            return new R2toR::DiscreteFunction_GPU(N, xLeft, h, h);
#endif

        throw "Error while instantiating Field: device not recognized.";
    }

    void *Builder::buildEquationSolver() {
        auto thePotential = new RtoR::AbsFunction;
        auto dphi = (R2toR::BoundaryCondition*)getBoundary();

        return new Fields::KleinGordon::Solver<R2toR::EquationState>(simulationConfig.numericConfig,
                                                                     *dphi,
                                                                     *thePotential,
                                                                     nullptr);
    }

    auto Builder::buildOpenGLOutput() -> R2toR::OutputOpenGL * {
        return new R2toR::OutputOpenGL(simulationConfig.numericConfig, -1, 1);
    }

    auto Builder::newFieldState() -> void * {
        auto u   = (R2toR::DiscreteFunction*)newFunctionArbitrary();
        auto du  = (R2toR::DiscreteFunction*)newFunctionArbitrary();

        return new R2toR::EquationState(u, du);
    }

    Stepper *Builder::buildStepper() {
        auto &solver = *(R2toR::EquationSolver*) buildEquationSolver();

        return new StepperRK4<typename R2toR::EquationState>(solver);
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