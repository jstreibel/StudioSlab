//
// Created by joao on 7/19/23.
//

#include "R2toRBuilder.h"

#include <utility>

#include "Core/Backend/BackendManager.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "EquationState.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h"

#include "Math/Numerics/ODE/Output/Format/OutputFormatterBase.h"
#include "Math/Numerics/ODE/Output/Format/BinarySOF.h"
#include "Math/Numerics/ODE/Output/Format/SpaceFilterBase.h"
#include "Math/Numerics/ODE/Output/Sockets/OutputHistoryToFile.h"
#include "Math/Numerics/ODE/Output/Sockets/OutputConsoleMonitor.h"

#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/R2toR/Output/Filters/DimensionReductionFilter.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"

#include "Models/KleinGordon/R2toR/KG-R2toRSolver.h"
#include "Math/Function/R2toR/Model/Operators/R2toRLaplacian.h"
#include "Graphics/Window/SlabWindowManager.h"

namespace Slab::Math::R2toR {

    Builder::Builder(const Str& name, Str description)
            : Models::KGRecipe(New<Models::KGNumericConfig>(), name, std::move(description)) {    }

    Vector<Pointer<Socket>> Builder::buildOutputSockets() {
        Vector<Pointer<Socket>> sockets;


        const auto shouldOutputOpenGL = *VisualMonitor;
        const auto shouldTrackHistory = !*noHistoryToFile;

        if (*VisualMonitor) Core::BackendManager::Startup("GLFW");
        else                Core::BackendManager::Startup("Headless");

        // outputManager->addOutputChannel(new LastOutputVTKVisualizer(numericParams, numericParams.getN()));

        RtoR2::StraightLine section;
        auto angleDegrees = 22.5;
        {
            const Real rMin = kg_numeric_config->getxMin();
            const Real rMax = kg_numeric_config->getxMax();
            const Real2D x0 = {rMin, .0}, xf = {rMax, .0};

            Rotation R;
            R = Rotation(M_PI*angleDegrees/180);
            section = RtoR2::StraightLine(R * x0, R * xf);
        }


        ///********************************************************************************************/
        if (shouldTrackHistory) {
            const Real t = kg_numeric_config->gett();
            const UInt outputResolutionX = *outputResolution;

            OutputFormatterBase *outputFilter = new BinarySOF;

            SpaceFilterBase *spaceFilter = new DimensionReductionFilter(
                    outputResolutionX, section, kg_numeric_config->getL());

            const auto N = (Real) kg_numeric_config->getN();
            const Real Np = outputResolutionX;
            const Real r = kg_numeric_config->getr();
            const auto stepsInterval = UInt(N / (Np * r));

            auto outputFileName = this->suggestFileName() + " section_tx_angle=" + ToStr(angleDegrees, 1);

            auto out = New<OutputHistoryToFile>(stepsInterval, spaceFilter, outputFileName, outputFilter);

            sockets.emplace_back(out);
        }
        ///********************************************************************************************/


        ///********************************************************************************************/
        if (shouldOutputOpenGL) {
            auto &backend = Slab::Graphics::GetGraphicsBackend();

            auto glOut = Graphics::BaseMonitor_ptr(this->buildOpenGLOutput());
            auto wm = New<Graphics::SlabWindowManager>();
            wm->addSlabWindow(glOut);
            backend.addAndOwnEventListener(wm);
            sockets.emplace_back(glOut);
        } else {
            /* O objetivo de relacionar o numero de passos para o Console Monitor com o do file output eh para que
             * ambos possam ficar sincronizados e o integrador possa rodar diversos passos antes de fazer o output. */
            IN conf = *kg_numeric_config;
            sockets.emplace_back(New<OutputConsoleMonitor>(conf.getn(), conf.gett()));
        }

        return sockets;

    }

    R2toR::NumericFunction_ptr Builder::newFunctionArbitrary() {
        const size_t N = kg_numeric_config->getN();
        const floatt xLeft = kg_numeric_config->getxMin();
        fix h = kg_numeric_config->geth();

        if (device_config == CPU)
            return DataAlloc<R2toR::NumericFunction_CPU>("IntegrationData [CPU]", N, N, xLeft, xLeft, h, h);

#if USE_CUDA
        else if (simulationConfig.dev == GPU)
            return New<R2toR::NumericFunction_GPU>(N, xLeft, h);
#endif

        throw Exception("Error while instantiating Field: device not recognized.");
    }

    Pointer<Base::LinearStepSolver> Builder::buildSolver() {
        auto thePotential = New<RtoR::AbsFunction>();
        auto dphi = getBoundary();

        using SolvySolver = Models::KGR2toR::KGR2toRSolver;
        auto Laplacian = New <R2toR::R2toRLaplacian>();

        return New<SolvySolver>(dphi, Laplacian, thePotential);
    }

    auto Builder::buildOpenGLOutput() -> R2toR::OutputOpenGL * {
        // t_max, max_steps, x_min, x_max, y_min, y_max
        IN conf = *kg_numeric_config;
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