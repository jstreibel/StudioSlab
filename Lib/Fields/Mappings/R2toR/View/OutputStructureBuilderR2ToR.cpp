//
// Created by joao on 10/8/21.
//

#include "OutputStructureBuilderR2ToR.h"

#include "Phys/Numerics/Program/NumericParams.h"
#include "Phys/Numerics/Allocator.h"
#include "Phys/Numerics/Output/Format/OutputFormatterBase.h"
#include "Phys/Numerics/Output/Format/BinarySOF.h"
#include "Phys/Numerics/Output/Format/SpaceFilterBase.h"
#include "Phys/Numerics/Output/Channel/OutputHistoryToFile.h"

#include "Fields/Mappings/RtoR2/StraightLine.h"
#include "Fields/Mappings/R2toR/View/Filters/DimensionReductionFilter.h"

#include "Studios/Backend/GLUT/GLUTBackend.h"
#include "Studios/Backend/Console/ConsoleBackend.h"

#include "R2toROutputOpenGLGeneric.h"

OutputManager*
OutputStructureBuilderR2toR::build(String outputFileName) {
    const auto shouldOutputOpenGL = *useOpenGL;
    const auto shouldOutputHistory = ! *noHistory;


    if(*useOpenGL) GLUTBackend::GetInstance();
    else ConsoleBackend::getSingleton();


    const NumericParams &p = Allocator::getInstance().getNumericParams();

    auto *outputManager = new OutputManager;


    RtoR2::StraightLine section1, section2;
    {
        const Real sqrt2 = sqrt(2.);
        const Real rMin = Allocator::getInstance().getNumericParams().getxLeft();
        const Real rMax = rMin + Allocator::getInstance().getNumericParams().getL();
        const Real2D x0 = {rMin, .0}, xf = {rMax, .0};

        Real theta = 0.0;

        Rotation R;
        R = Rotation(theta + .5 * M_PI);
        section1 = RtoR2::StraightLine(R * x0, R * xf);
        R = Rotation(theta);
        section2 = RtoR2::StraightLine(R * x0, R * xf);
    }


    ///********************************************************************************************/
    int fileOutputStepsInterval = -1;
    if(shouldOutputHistory)
    {
        const double t=p.gett();
        const PosInt outputResolutionX = *outputResolution;

        OutputFormatterBase *outputFilter = new BinarySOF;

        SpaceFilterBase *spaceFilter = new DimensionReductionFilter(
                Allocator::getInstance().getNumericParams().getN(), section1);

        const auto N = (Real) Allocator::getInstance().getNumericParams().getN();
        const Real Np = outputResolutionX;
        const Real r = Allocator::getInstance().getNumericParams().getr();
        const auto stepsInterval = PosInt(N/(Np*r));

        auto fileName = outputFileName + "-N=" + ToString(N, 0);

        OutputChannel *out = new OutputHistoryToFile(stepsInterval, spaceFilter, t, fileName, outputFilter);

        fileOutputStepsInterval = out->getNSteps();
        outputManager->addOutputChannel(out);
    }
    ///********************************************************************************************/


    ///********************************************************************************************/
    if(shouldOutputOpenGL) {
        std::cout << std::endl << "Outputting OpenGL" << std::endl;
        GLUTBackend *glutBackend = GLUTBackend::GetInstance(); // GLUTBackend precisa ser instanciado, de preferencia, antes dos OutputOpenGL.

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
    const double phiMin = -0.25;
    const double phiMax = 0.25;
    const Real xLeft = Allocator::getInstance().getNumericParams().getxLeft();
    const Real xRight = xLeft + Allocator::getInstance().getNumericParams().getL();

    return new R2toR::OutputOpenGL(xLeft, xRight, xLeft, xRight, phiMin, phiMax);
}


