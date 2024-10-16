//
// Created by joao on 7/19/23.
//

#include "R2toRBuilder.h"

#include <utility>

#include "Core/Backend/BackendManager.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionGPU.h"
#include "EquationState.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h"

#include "Math/Numerics/Output/Format/OutputFormatterBase.h"
#include "Math/Numerics/Output/Format/BinarySOF.h"
#include "Math/Numerics/Output/Format/SpaceFilterBase.h"
#include "Math/Numerics/Output/Plugs/OutputHistoryToFile.h"
#include "Math/Numerics/Output/Plugs/OutputConsoleMonitor.h"

#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/R2toR/Output/Filters/DimensionReductionFilter.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"

#include "Models/KleinGordon/R2toR/KG-R2toRSolver.h"
#include "Math/Function/R2toR/Model/Operators/R2toRLaplacian.h"

namespace Slab::Math::R2toR {

    Builder::Builder(const Str& name, Str description)
            : Models::KGBuilder(name, std::move(description)) {    }

    Pointer<OutputManager> Builder::buildOutputManager() {
        const auto shouldOutputOpenGL = *VisualMonitor;
        const auto shouldTrackHistory = !*noHistoryToFile;

        if (*VisualMonitor) Core::BackendManager::Startup("GLFW");
        else                Core::BackendManager::Startup("Headless");

        auto outputManager = New <OutputManager> (simulationConfig.numericConfig.getn());

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

            auto out = New<OutputHistoryToFile>(stepsInterval, spaceFilter, t, outputFileName, outputFilter);

            fileOutputStepsInterval = out->getnSteps();
            outputManager->addOutputChannel(out);
        }
        ///********************************************************************************************/


        ///********************************************************************************************/
        if (shouldOutputOpenGL) {
            GraphicBackend &backend = Core::BackendManager::GetGUIBackend();

            auto glOut = Graphics::BaseMonitor_ptr(this->buildOpenGLOutput());
            backend.addEventListener(glOut);
            outputManager->addOutputChannel(glOut);
        } else {
            /* O objetivo de relacionar o numero de passos para o Console Monitor com o do file output eh para que
             * ambos possam ficar sincronizados e o integrador possa rodar diversos passos antes de fazer o output. */
            IN conf = simulationConfig.numericConfig;
            outputManager->addOutputChannel(
                    New<OutputConsoleMonitor>(conf.getn(), conf.gett(), conf.getr()));
        }

        return outputManager;

    }

    R2toR::NumericFunction_ptr Builder::newFunctionArbitrary() {
        const size_t N = simulationConfig.numericConfig.getN();
        const floatt xLeft = simulationConfig.numericConfig.getxMin();
        fix h = simulationConfig.numericConfig.geth();

        if (simulationConfig.dev == CPU)
            return New<R2toR::NumericFunction_CPU>(N, N, xLeft, xLeft, h, h);

#if USE_CUDA
        else if (simulationConfig.dev == GPU)
            return New<R2toR::NumericFunction_GPU>(N, xLeft, h);
#endif

        throw "Error while instantiating Field: device not recognized.";
    }

    Base::Solver_ptr Builder::buildEquationSolver() {
        auto thePotential = New<RtoR::AbsFunction>();
        auto dphi = getBoundary();

        using SolvySolver = Models::KGR2toR::KGR2toRSolver;
        auto Laplacian = New <R2toR::R2toRLaplacian>();

        return New<SolvySolver>(simulationConfig.numericConfig, dphi, Laplacian, thePotential);
    }

    auto Builder::buildOpenGLOutput() -> R2toR::OutputOpenGL * {
        // t_max, max_steps, x_min, x_max, y_min, y_max
        IN conf = simulationConfig.numericConfig;
        Real x_min = conf.getxMin();
        Real x_max = conf.getxMax();
        return new R2toR::OutputOpenGL(conf.gett(), conf.getn(), x_min, x_max, x_min, x_max);
    }

    auto Builder::newFieldState() -> R2toR::EquationState_ptr {
        auto u   = newFunctionArbitrary();
        auto du  = newFunctionArbitrary();

        return New<R2toR::EquationState>(u, du);
    }

    R2toR::EquationState_ptr Builder::getInitialState() {
        RtoR::NullFunction nullFunction;
        R2toR::FunctionAzimuthalSymmetry fullNull(&nullFunction, 1, 0, 0, false);

        auto u_0 = newFieldState();

        u_0->setPhi(fullNull);
        u_0->setDPhiDt(fullNull);

        return u_0;
    }
};