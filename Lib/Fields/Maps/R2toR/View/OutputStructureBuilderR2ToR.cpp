//
// Created by joao on 10/8/21.
//

#include "OutputStructureBuilderR2ToR.h"

#include <View/OutputFormatter/BinarySOF.h>
#include <Lib/Fields/Maps/RtoR2/StraightLine.h>
#include <Lib/Fields/Maps/RtoR/View/OutputSnapshots.h>
#include <Lib/Fields/Maps/R2toR/View/Filters/DimensionReductionFilter.h>
#include <Apps/Backend/GLUTBackend.h>
#include <Lib/Fields/Maps/R2toR/View/R2toROutputOpenGLGeneric.h>
#include "View/Base/OutputHistoryToFile.h"
#include "View/Base/OutputConsoleMonitor.h"
#include "View/OutputFormatter/CustomStringSeparatedSOF.h"
#include "View/SpaceFilter/ResolutionReductionFilter.h"

OutputManager*
OutputStructureBuilderR2toR::build() {
    //buildCommon(outputManager, inputParameters, vm);
//
    //const auto shouldOutputOpenGL = useOpenGL;
    //const auto shouldOutputHistory = !GET_FLAG("noOut");
    //const auto shouldOutputHistoryInBinaryFormat = GET_FLAG("output_binary");
//
    //const NumericParams &p = Allocator::getInstance().getNumericParams();
//
//
    //RtoR2::StraightLine section1, section2;
    //{
    //    const Real sqrt2 = sqrt(2.);
    //    const Real rMin = Allocator::getInstance().getNumericParams().getxLeft();
    //    const Real rMax = rMin + Allocator::getInstance().getNumericParams().getL();
    //    const Real2D x0 = {rMin, .0}, xf = {rMax, .0};
    //    Real theta = 0.0;
    //    auto thetaParam = inputParameters.find("sw_theta");
    //    if (thetaParam != inputParameters.end()) {
    //        const auto *param = dynamic_cast<const DoubleParameter *>(thetaParam->second);
    //        theta = -param->val;
    //    }
    //    Rotation R;
    //    R = Rotation(theta + .5 * M_PI);
    //    section1 = RtoR2::StraightLine(R * x0, R * xf);
    //    R = Rotation(theta);
    //    section2 = RtoR2::StraightLine(R * x0, R * xf);
    //}
//
//
    ///********************************************************************************************/
    //int fileOutputStepsInterval = -1;
    //if(shouldOutputHistory)
    //{
    //    String ext = ".osc";
//
    //    const double Tf=p.gett();
    //    const PosInt outputResolutionX = GET("outN", int);
//
//
    //    OutputFormatterBase *outputFilter = nullptr;
    //    if(shouldOutputHistoryInBinaryFormat){
    //        ext += 'b';
    //        outputFilter = new BinarySOF;
    //    }
    //    else outputFilter = new CustomStringSeparatedSOF;
//
//
    //    SpaceFilterBase *spaceFilter = new DimensionReductionFilter(Allocator::getInstance().getNumericParams().getN(), section1);
//
//
    //    const auto N = (Real) Allocator::getInstance().getNumericParams().getN();
    //    const Real Np = outputResolutionX;
    //    const Real r = Allocator::getInstance().getNumericParams().getr();
    //    const auto stepsInterval = PosInt(N/(Np*r));
//
    //    OutputChannel *out = new OutputHistoryToFile(stepsInterval, spaceFilter, Tf, fileNameBase + ext,
    //                                              inputParameters, outputFilter);
    //    fileOutputStepsInterval = out->getNSteps();
    //    outputManager.addOutputChannel(out);
    //}
    ///********************************************************************************************/
//
//
    ///********************************************************************************************/
    //if(shouldOutputOpenGL) {
    //    std::cout << std::endl << "Outputting OpenGL" << std::endl;
    //    GLUTBackend *glutBackend = GLUTBackend::getSingleton(); // GLUTBackend precisa ser instanciado, de preferencia, antes dos OutputOpenGL.
//
//
    //    const double phiMin = -0.08;
    //    const double phiMax = 0.08;
    //    const Real xLeft = Allocator::getInstance().getNumericParams().getxLeft();
    //    const Real xRight = xLeft + Allocator::getInstance().getNumericParams().getL();
//
    //    auto *outputOpenGL = new R2toR::OutputOpenGL(xLeft, xRight, xLeft, xRight, phiMin, phiMax);
//
//
    //    glutBackend->setOpenGLOutput(outputOpenGL);
    //    // outGL->output(dummyInfo); // stop flicker?
    //    outputManager.addOutputChannel(outputOpenGL);
    //}
    //else/* O objetivo de relacionar o numero de passos para o Console Monitor com o do file output eh para que
    //     * ambos possam ficar sincronizados e o integrador possa rodar diversos passos antes de fazer o output. */
    //    addConsoleMonitor(outputManager, fileOutputStepsInterval>0 ? fileOutputStepsInterval*25 : int(p.getn()/40));
}

//void OutputStructureBuilderR2toR::_addSpecificCommandLineOptions(CLOptionsDescription &options) {
//    options.add_options()("noOut,o", "Don't output history to file.")
//            ("output_binary,b", "Output history data in binary getFormatDescription")
//            ("out", po::value<String>()->default_value(""), "Filename of history and/or snapshot output (will differ by extension).")
//            ("outN", po::value<int>()->default_value(512), "Output X resolution of history output.")
//            ("avg", "Flag to take average of region when outputting history.");
//}
