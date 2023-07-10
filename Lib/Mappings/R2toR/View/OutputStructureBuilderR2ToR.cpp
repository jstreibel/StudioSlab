//
// Created by joao on 10/8/21.
//

#include "OutputStructureBuilderR2ToR.h"

#include "Phys/Numerics/Program/NumericParams.h"
#include "Phys/Numerics/Allocator.h"
#include "Phys/Numerics/Output/Format/OutputFormatterBase.h"
#include "Phys/Numerics/Output/Format/BinarySOF.h"
#include "Phys/Numerics/Output/Format/SpaceFilterBase.h"
#include "Phys/Numerics/Output/Plugs/OutputHistoryToFile.h"

#include "Mappings/R2toR/View/Filters/DimensionReductionFilter.h"

#include "Base/Backend/GLUT/GLUTBackend.h"
#include "Base/Backend/Console/ConsoleBackend.h"

#include "R2toROutputOpenGLGeneric.h"

R2toR::OutputSystem::Builder::Builder(Str name, Str description) : Numerics::OutputSystem::Builder(name, description) {
}

OutputManager*
R2toR::OutputSystem::Builder::build(Str outputFileName) {
    const auto shouldOutputOpenGL = *VisualMonitor;
    const auto shouldTrackHistory = ! *noHistoryToFile;


    if(*VisualMonitor) Backend::Initialize<GLUTBackend>();
    else Backend::Initialize<ConsoleBackend>();


    const NumericParams &p = Numerics::Allocator::GetInstance().getNumericParams();

    auto *outputManager = new OutputManager;


    RtoR2::StraightLine section1, section2;
    {
        const Real sqrt2 = sqrt(2.);
        const Real rMin = Numerics::Allocator::GetInstance().getNumericParams().getxLeft();
        const Real rMax = rMin + Numerics::Allocator::GetInstance().getNumericParams().getL();
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
        const Real t=p.gett();
        const PosInt outputResolutionX = *outputResolution;

        OutputFormatterBase *outputFilter = new BinarySOF;

        SpaceFilterBase *spaceFilter = new DimensionReductionFilter(
                Numerics::Allocator::GetInstance().getNumericParams().getN(), section1);

        const auto N = (Real) Numerics::Allocator::GetInstance().getNumericParams().getN();
        const Real Np = outputResolutionX;
        const Real r = Numerics::Allocator::GetInstance().getNumericParams().getr();
        const auto stepsInterval = PosInt(N/(Np*r));

        auto fileName = outputFileName + "-N=" + ToStr(N, 0);

        Numerics::OutputSystem::Socket *out = new OutputHistoryToFile(stepsInterval, spaceFilter, t, fileName, outputFilter);

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
        addConsoleMonitor(*outputManager,
                          fileOutputStepsInterval > 0 ? fileOutputStepsInterval * 25 : int(p.getn() / 40));
    }

    return outputManager;
}

auto R2toR::OutputSystem::Builder::buildOpenGLOutput() -> R2toR::OutputOpenGL * {
    const Real phiMin = -0.125;
    const Real phiMax = 0.25;
    const Real xLeft = Numerics::Allocator::GetInstance().getNumericParams().getxLeft();
    const Real xRight = xLeft + Numerics::Allocator::GetInstance().getNumericParams().getL();

    return new R2toR::OutputOpenGL(xLeft, xRight, xLeft, xRight, phiMin, phiMax);
}




