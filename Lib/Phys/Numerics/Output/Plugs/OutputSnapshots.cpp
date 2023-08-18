//
// Created by joao on 7/22/19.
//

#include "OutputSnapshots.h"

#include "Phys/Numerics/Output/Format/CustomStringSeparatedSOF.h"
#include "Phys/Numerics/Output/Plugs/Socket.h"
#include "Base/Tools/Log.h"

#include <Base/Controller/Interface/InterfaceManager.h>


OutputSnapshot::OutputSnapshot(const NumericConfig &params, const Str &customFileDescription, const size_t T_fileNamePrecision)
        : Numerics::OutputSystem::Socket(params, "Snapshot output", 1), customFileDescription(customFileDescription),
          T_fileNamePrecision(T_fileNamePrecision)
{ }

OutputSnapshot::~OutputSnapshot() = default;

void OutputSnapshot::addSnapshotStep(const size_t snapshotStep) {
    snapSteps.push_back(snapshotStep);

    Log::Attention() << "A snapshot will be taken at the step " << snapshotStep << Log::Flush;
}

void OutputSnapshot::doOutput(const OutputPacket &outInfo, const Str &customFileDescription,
                              const size_t T_fileNamePrecision) {
    StringStream filePhiNameStream;
    filePhiNameStream.setf(std::ios::fixed, std::ios::floatfield);
    filePhiNameStream.precision(T_fileNamePrecision);

    const Real t = outInfo.getSimTime ();

    if(customFileDescription != "")
        filePhiNameStream << customFileDescription;
    else
        filePhiNameStream << "./snapshot-t=" << t;
    filePhiNameStream  << ".oscs";

    Str fileName = filePhiNameStream.str();
    auto &log = Log::Info();
    log.setf(std::ios::fixed, std::ios::floatfield);
    log.precision(4);

    log << "Saving snapshot for t = " << t << " in \'" << fileName << "\'... " << Log::Flush;
    _outputToFile(outInfo.getSpaceData(), t, fileName);
    Log::Success() << "Snapshot saved! File '" << fileName << "'" << Log::Flush;
}

void OutputSnapshot::_outputToFile(DiscreteSpacePair spaceData, Real t, const Str &fileName) {
    std::ofstream file;
    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);

    try {
        file.open(fileName);
        Log::Note() << "Opened file \"" << fileName << "\"" << Log::Flush;

        EqStateOutputInterface::format = EqStateOutputInterface::PythonDictionaryEntry;
        EqStateOutputInterface::fDataOutType = EqStateOutputInterface::PhiAndDPhiDt;

        CustomStringSeparatedSOF formatter(", ");
        file << "# {" << InterfaceManager::getInstance().renderAsPythonDictionaryEntries() /*<< ", " */ << "\"t\": " << t
             << ", \"phi\": (" << formatter(*spaceData.first) << ")"
             << ", \"dPhiDt\": (" << formatter(*spaceData.second) << ")}";

        file.flush();
    } catch (std::system_error& e) {
        Log::Error() << "File not saved. std::system_error::code().message(): "
                        "\"" << e.code().message() << "\"." << Log::Flush;
    }

    file.close();
    Log::Note() << "Closed file \"" << fileName << "\"" << Log::Flush;
}

bool OutputSnapshot::shouldOutput(const Real, const long unsigned timeStep) {
    for(const size_t step : snapSteps)
        if(step == timeStep) return true;

    return false;
}

size_t OutputSnapshot::computeNextRecStep() {
    size_t smallest = snapSteps.back();
    for(auto step : snapSteps) if(step < smallest) smallest = step;

    return smallest;
}


