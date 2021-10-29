//
// Created by joao on 10/8/21.
//

// #include <RtoR/View/OutputSnapshots.h>
#include "Channel/OutputConsoleMonitor.h"
#include "OutputStructureBuilderBase.h"

#include "OutputManager.h"


OutputStructureBuilderBase::OutputStructureBuilderBase(String generalDescription)
    : Interface(generalDescription, true)
{
    addParameters({&noHistory, &outputResolution, &useOpenGL, /*&takeSnapshot, &snapshotTime, */ });
}

//void OutputStructureBuilderBase::buildCommon(OutputManager &outputManager) {
//
//
//    //fileNameBase = GET("out", String);
//    //if(fileNameBase.empty()){
//    //    auto fNameSugestion = inputParameters.find("filenameSugestion");
//    //    if(fNameSugestion != inputParameters.end()){
//    //        fileNameBase = fNameSugestion->second->valueToString();
//    //    } else {
//    //        std::ostringstream stringStream;
//    //        for(auto paramPair : inputParameters){
//    //            auto param = paramPair.second;
//    //            stringStream << "-" << param->getCommandLineArgName() << "=" << param->valueToString();
//    //        }
//    //        fileNameBase = stringStream.str().substr(1); // pra eliminar o char '-' do inicio.
//    //    }
//    //}
////
////
//    ///********************************************************************************************/
//    //const auto shouldOutputSnapshot = GET_FLAG("snapshot");
//    //if(shouldOutputSnapshot)
//    //{
//    //    const String ext = ".oscs";
//    //    auto *snapshotOutput = new OutputSnapshot(fileNameBase + ext);
//    //    auto n = -1;
//    //    throw "Snapshot not implemented";
//    //    snapshotOutput->addSnapshotStep(GET_FALLBACK("snapshotTime", double, n));
//    //    outputManager.addOutputChannel(snapshotOutput);
//    //}
//    ///********************************************************************************************/
//}

void OutputStructureBuilderBase::addConsoleMonitor(OutputManager &outputManager, int nSteps) {
    OutputChannel *out = new OutputConsoleMonitor(nSteps, true);
    outputManager.addOutputChannel(out);
}

