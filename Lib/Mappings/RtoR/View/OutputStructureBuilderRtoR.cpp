//
// Created by joao on 10/8/21.
//

#include "OutputStructureBuilderRtoR.h"

#include "Mappings/RtoR/View/Graphics/RtoROutputOpenGL.h"
#include "Mappings/RtoR/View/Graphics/RtoROutGLStatistic.h"



// #include <Phys/DiffMath/Numerics/Output/Channel/OutputSnapshot.h>
#include "Phys/Numerics/Output/OutputManager.h"

#include "Phys/Numerics/Output/Format/BinarySOF.h"
#include "Phys/Numerics/Output/Format/CustomStringSeparatedSOF.h"
#include "Phys/Numerics/Output/Format/ResolutionReductionFilter.h"

#include "Phys/Numerics/Output/Channel/OutputHistoryToFile.h"
#include "Phys/Numerics/Output/Channel/OutputConsoleMonitor.h"


#include "Base/Backend/GLUT/GLUTBackend.h"
#include "Base/Backend/Console/ConsoleBackend.h"



OutputStructureBuilderRtoR::OutputStructureBuilderRtoR() : OutputStructureBuilderBase() {

}

auto
OutputStructureBuilderRtoR::build(String outputFileName) -> OutputManager * {
    const auto shouldOutputOpenGL = *OpenGLMonitor;
    const auto shouldOutputHistory = ! *noHistoryToFile;


    if(*OpenGLMonitor) GLUTBackend::GetInstance();
    else ConsoleBackend::getSingleton();


    const NumericParams &p = Numerics::Allocator::getInstance().getNumericParams();

    auto *outputManager = new OutputManager;

    /********************************************************************************************/
    int fileOutputStepsInterval = -1;
    if(shouldOutputHistory)
    {
        const double Tf=p.gett();

        OutputFormatterBase *outputFilter = new BinarySOF;
        //OutputFormatterBase *outputFilter = new CustomStringSeparatedSOF;

        auto *spaceFilter = new ResolutionReductionFilter(DimensionMetaData({(unsigned)*outputResolution}));

        const auto N = (Real) Numerics::Allocator::getInstance().getNumericParams().getN();
        const Real Np = *outputResolution;
        const Real r = Numerics::Allocator::getInstance().getNumericParams().getr();
        const auto stepsInterval = PosInt(N/(Np*r));

        outputFileName += String("-N=") + ToString(N, 0);

        OutputChannel *out = new OutputHistoryToFile(stepsInterval, spaceFilter, Tf,
                                                     outputFileName, outputFilter);
        fileOutputStepsInterval = out->getNSteps();
        outputManager->addOutputChannel(out);
    }
    /********************************************************************************************/


    /********************************************************************************************/
    if(shouldOutputOpenGL) {
        std::cout << std::endl << "Outputting OpenGL" << std::endl;
        GLUTBackend *glutBackend = GLUTBackend::GetInstance(); // GLUTBackend precisa ser instanciado, de preferencia, antes dos OutputOpenGL.


        const double phiMin = -0.08;
        const double phiMax = 0.08;
        const Real xLeft = Numerics::Allocator::getInstance().getNumericParams().getxLeft();
        const Real xRight = xLeft + Numerics::Allocator::getInstance().getNumericParams().getL();

        //Base::OutputOpenGL *outputOpenGL = new RtoR::OutputOpenGL(xLeft, xRight, phiMin, phiMax);
        Base::OutputOpenGL *outputOpenGL = buildOpenGLOutput();

        glutBackend->setOpenGLOutput(outputOpenGL);
        // outGL->output(dummyInfo); // stop flicker?
        outputManager->addOutputChannel(outputOpenGL);
    }
    else
        /* O objetivo de relacionar o numero de passos para
         * o Console Monitor com o do file output eh para que
         * ambos possam ficar sincronizados e o integrador
         * possa rodar diversos passos antes de fazer o output. */
        addConsoleMonitor(*outputManager, fileOutputStepsInterval>0 ? fileOutputStepsInterval*25 : int(p.getn()/40));

    return outputManager;
}

auto OutputStructureBuilderRtoR::buildOpenGLOutput() -> RtoR::OutputOpenGL * {
    return new RtoR::OutGLStatistic();
}
