//
// Created by joao on 10/8/21.
//

#include "OutputStructureBuilderRtoR.h"

#include "../View/Graphics/RtoROutputOpenGL.h"



// #include <Studios/CoreMath/Numerics/Output/Channel/OutputSnapshot.h>
#include <Studios/CoreMath/Numerics/Output/OutputManager.h>

#include <Studios/CoreMath/Numerics/Output/Format/BinarySOF.h>
#include <Studios/CoreMath/Numerics/Output/Format/CustomStringSeparatedSOF.h>
#include <Studios/CoreMath/Numerics/Output/Format/ResolutionReductionFilter.h>

#include <Studios/CoreMath/Numerics/Output/Channel/OutputHistoryToFile.h>
#include <Studios/CoreMath/Numerics/Output/Channel/OutputConsoleMonitor.h>


#include <Studios/Backend/Graphic/GLUTBackend.h>
#include <Studios/Backend/Console/ConsoleBackend.h>



OutputStructureBuilderRtoR::OutputStructureBuilderRtoR() : OutputStructureBuilderBase() {

}

auto
OutputStructureBuilderRtoR::build(String outputFileName) -> OutputManager * {
    const auto shouldOutputOpenGL = *useOpenGL;
    const auto shouldOutputHistory = ! *noHistory;


    if(*useOpenGL) GLUTBackend::GetInstance();
    else ConsoleBackend::getSingleton();


    const NumericParams &p = Allocator::getInstance().getNumericParams();

    auto *outputManager = new OutputManager;

    /********************************************************************************************/
    int fileOutputStepsInterval = -1;
    if(shouldOutputHistory)
    {
        const double Tf=p.gett();

        OutputFormatterBase *outputFilter = new BinarySOF;

        auto *spaceFilter = new ResolutionReductionFilter(DimensionMetaData({(unsigned)*outputResolution}));

        const auto N = (Real) Allocator::getInstance().getNumericParams().getN();
        const Real Np = *outputResolution;
        const Real r = Allocator::getInstance().getNumericParams().getr();
        const auto stepsInterval = PosInt(N/(Np*r));

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
        const Real xLeft = Allocator::getInstance().getNumericParams().getxLeft();
        const Real xRight = xLeft + Allocator::getInstance().getNumericParams().getL();

        Base::OutputOpenGL *outputOpenGL = new RtoR::OutputOpenGL(xLeft, xRight, phiMin, phiMax);

        glutBackend->setOpenGLOutput(outputOpenGL);
        // outGL->output(dummyInfo); // stop flicker?
        outputManager->addOutputChannel(outputOpenGL);
    }
    else
        /* O objetivo de relacionar o numero de passos para o Console Monitor com o do file output eh para que
         * ambos possam ficar sincronizados e o integrador possa rodar diversos passos antes de fazer o output. */
        addConsoleMonitor(*outputManager, fileOutputStepsInterval>0 ? fileOutputStepsInterval*25 : int(p.getn()/40));

    return outputManager;
}
