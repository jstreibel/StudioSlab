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

#include "Mappings/RtoR2/StraightLine.h"
#include "Mappings/R2toR/View/Filters/DimensionReductionFilter.h"

#include "Base/Backend/GLUT/GLUTBackend.h"
#include "Base/Backend/Console/ConsoleBackend.h"

#include "R2toROutputOpenGLGeneric.h"
#include "Phys/Numerics/Output/Plugs/Plug.h"
#include "Phys/Numerics/Output/Plugs/Plug.h"

OutputManager*
OutputStructureBuilderR2toR::build(String outputFileName) {
    const auto shouldOutputOpenGL = *OpenGLMonitor;
    const auto shouldTrackHistory = ! *noHistoryToFile;


    if(*OpenGLMonitor) GLUTBackend::GetInstance();
    else ConsoleBackend::getSingleton();


    const NumericParams &p = Numerics::Allocator::getInstance().getNumericParams();

    auto *outputManager = new OutputManager;


    RtoR2::StraightLine section1, section2;
    {
        const Real sqrt2 = sqrt(2.);
        const Real rMin = Numerics::Allocator::getInstance().getNumericParams().getxLeft();
        const Real rMax = rMin + Numerics::Allocator::getInstance().getNumericParams().getL();
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
        const double t=p.gett();
        const PosInt outputResolutionX = *outputResolution;

        OutputFormatterBase *outputFilter = new BinarySOF;

        SpaceFilterBase *spaceFilter = new DimensionReductionFilter(
                Numerics::Allocator::getInstance().getNumericParams().getN(), section1);

        const auto N = (Real) Numerics::Allocator::getInstance().getNumericParams().getN();
        const Real Np = outputResolutionX;
        const Real r = Numerics::Allocator::getInstance().getNumericParams().getr();
        const auto stepsInterval = PosInt(N/(Np*r));

        auto fileName = outputFileName + "-N=" + ToString(N, 0);

        Numerics::OutputSystem::Plug *out = new OutputHistoryToFile(stepsInterval, spaceFilter, t, fileName, outputFilter);

        fileOutputStepsInterval = out->getNSteps();
        outputManager->addOutputChannel(out);
    }
    ///********************************************************************************************/


    ///********************************************************************************************/
    if(shouldOutputOpenGL) {
        std::cout << std::endl << "Outputting OpenGL" << std::endl;
        GLUTBackend *glutBackend = GLUTBackend::GetInstance(); // GLUTBackend precisa ser instanciado, de preferencia, antes dos OutputOpenGL.
        if(*OpenGLMonitor_startPaused) glutBackend->pause();
        else glutBackend->resume();
        glutBackend->setStepsPerFrame(*OpenGLMonitor_stepsPerIdleCall);

        auto glOut = this->buildOpenGLOutput();


        glutBackend->setOpenGLOutput(glOut);
        outputManager->addOutputChannel(glOut);
    }
    else {
        /* O objetivo de relacionar o numero de passos para o Console Monitor com o do file output eh para que
         * ambos possam ficar sincronizados e o integrador possa rodar diversos passos antes de fazer o output. */
        addConsoleMonitor(*outputManager,
                          fileOutputStepsInterval > 0 ? fileOutputStepsInterval * 25 : int(p.getn() / 40));
    }

    return outputManager;
}

auto OutputStructureBuilderR2toR::buildOpenGLOutput() -> R2toR::OutputOpenGL * {
    const double phiMin = -0.125;
    const double phiMax = 0.25;
    const Real xLeft = Numerics::Allocator::getInstance().getNumericParams().getxLeft();
    const Real xRight = xLeft + Numerics::Allocator::getInstance().getNumericParams().getL();

    return new R2toR::OutputOpenGL(xLeft, xRight, xLeft, xRight, phiMin, phiMax);
}


