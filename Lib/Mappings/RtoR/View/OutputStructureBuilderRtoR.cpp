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

#include "Phys/Numerics/Output/Plugs/OutputHistoryToFile.h"
#include "Phys/Numerics/Output/Plugs/OutputConsoleMonitor.h"


#include "Base/Backend/GLUT/GLUTBackend.h"
#include "Base/Backend/Console/ConsoleBackend.h"
#include "Phys/Numerics/Output/Plugs/Socket.h"


OutputStructureBuilderRtoR::OutputStructureBuilderRtoR(Str name, Str descr) : Builder(name, descr) {

}

auto
OutputStructureBuilderRtoR::build(Str outputFileName) -> OutputManager * {
    const auto shouldOutputOpenGL = *VisualMonitor;
    const auto shouldOutputHistory = ! *noHistoryToFile;


    if(*VisualMonitor) Backend::Initialize<GLUTBackend>();
    else Backend::Initialize<ConsoleBackend>();

    const NumericParams &p = Numerics::Allocator::GetInstance().getNumericParams();

    auto *outputManager = new OutputManager;

    /********************************************************************************************/
    int fileOutputStepsInterval = -1;
    if(shouldOutputHistory)
    {
        const Real Tf=p.gett();

        OutputFormatterBase *outputFilter = new BinarySOF;
        //OutputFormatterBase *outputFilter = new CustomStringSeparatedSOF;

        auto *spaceFilter = new ResolutionReductionFilter(DimensionMetaData({(unsigned)*outputResolution}));

        const auto N = (Real) Numerics::Allocator::GetInstance().getNumericParams().getN();
        const Real Np = *outputResolution;
        const Real r = Numerics::Allocator::GetInstance().getNumericParams().getr();
        const auto stepsInterval = PosInt(N/(Np*r));

        outputFileName += Str("-N=") + ToStr(N, 0);

        Numerics::OutputSystem::Socket *out = new OutputHistoryToFile(stepsInterval, spaceFilter, Tf,
                                                                      outputFileName, outputFilter);
        fileOutputStepsInterval = out->getnSteps();
        outputManager->addOutputChannel(out);
    }
    /********************************************************************************************/


    /********************************************************************************************/
    if(shouldOutputOpenGL) {
        auto &glutBackend = Backend::GetInstanceSuper<GLUTBackend>(); // GLUTBackend precisa ser instanciado, de preferencia, antes dos OutputOpenGL.


        const Real phiMin = -0.08;
        const Real phiMax = 0.08;
        const Real xLeft = Numerics::Allocator::GetInstance().getNumericParams().getxLeft();
        const Real xRight = xLeft + Numerics::Allocator::GetInstance().getNumericParams().getL();

        //Base::OutputOpenGL *outputOpenGL = new RtoR::OutputOpenGL(xLeft, xRight, phiMin, phiMax);
        auto outputOpenGL = buildOpenGLOutput();
        glutBackend.addWindow(std::shared_ptr<Window>(outputOpenGL));
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
