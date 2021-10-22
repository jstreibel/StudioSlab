//
// Created by joao on 10/8/21.
//

#include "OutputChannel.h"


OutputChannel::OutputChannel(int nStepsInterval) : nStepsBetweenRecordings(nStepsInterval) {      }


auto OutputChannel::getLastT() -> double { return lastInfo.getT(); }
auto OutputChannel::getNSteps() const -> int { return nStepsBetweenRecordings; }


//auto OutputChannel::buildAHeader() -> String {
//    // TODO "Colocar tambem informacoes sobre o modelo, etc.";
//
//    throw "Here use some cenral Parameter (Interface) manager to do whats below. ";

    //const UserParamMap &modelMTMap = Allocator::getInstance().getParameters();
    //const UserParamMap &paramMTMap = Allocator::getInstance().getNumericParams().getParametersMap();
    //const UserParamMap &devMTMap = Allocator::getInstance().getDevice().getParametersMap();
//
    //StringStream stringStream;
//
    //for(letcr p : modelMTMap) stringStream << "\"" << p.first << "\": " << p.second << ", ";
    //for(letcr p : paramMTMap) stringStream << "\"" << p.first << "\": " << p.second << ", ";
    //for(letcr p : devMTMap) stringStream   << "\"" << p.first << "\": " << p.second << ", ";
//
    //return stringStream.str();
//}


auto OutputChannel::computeNextRecStep() -> size_t {
    const size_t lastStep = lastInfo.getSteps();

    return lastStep+size_t(nStepsBetweenRecordings);
}


auto OutputChannel::shouldOutput(const double t, const long unsigned timestep) -> bool {

#if SHOULD_OUTPUT___MODE == INT_BASED
    (void)t;
    return ! bool(timestep%unsigned(nStepsBetweenRecordings));
#elif SHOULD_OUTPUT___MODE == FLOAT_BASED
    return abs(T-lastT) > abs(recDT);
#endif

    throw "Boundary not implemented.";
}

void OutputChannel::output(const OutputPacket &outInfo){
    _out(outInfo);
    lastInfo = outInfo;
}