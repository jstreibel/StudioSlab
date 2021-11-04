//
// Created by joao on 7/22/19.
//

#include "OutputSnapshots.h"

#include "Phys/Numerics/Output/Format/CustomStringSeparatedSOF.h"

#include <Common/Workaround/StringStream.h>
#include <Studios/Controller/Interface/InterfaceManager.h>


OutputSnapshot::OutputSnapshot(const String &customFileDescription, const size_t T_fileNamePrecision)
        : OutputChannel(1), customFileDescription(customFileDescription),
          T_fileNamePrecision(T_fileNamePrecision)
{ }

OutputSnapshot::~OutputSnapshot() = default;

void OutputSnapshot::addSnapshotStep(const size_t snapshotStep) {
    snapSteps.push_back(snapshotStep);

    std::cout << "A snapshot will be taken at the step " << snapshotStep << std::endl;
}

void OutputSnapshot::doOutput(const OutputPacket &outInfo, const String &customFileDescription,
                              const size_t T_fileNamePrecision) {
    StringStream filePhiNameStream;
    filePhiNameStream.oStream.setf(std::ios::fixed, std::ios::floatfield);
    filePhiNameStream.oStream.precision(T_fileNamePrecision);

    const double t = outInfo.getT();

    if(customFileDescription != "")
        filePhiNameStream << customFileDescription;
    else
        filePhiNameStream << "./snapshot-t=" << t;
    filePhiNameStream  << ".oscs";

    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision(4);

    String fileName = filePhiNameStream.str();
    std::cout << "\nSaving snapshot for t = " << outInfo.getT() << " in \'" << fileName << "\'... " << std::flush;
    _outputToFile(outInfo.getSpaceData(), t, fileName);
}

void OutputSnapshot::_outputToFile(DiscreteSpacePair spaceData, double t, const String &fileName) {
    std::ofstream file;
    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);

    try {
        file.open(fileName);

        FStateOutputInterface::format = FStateOutputInterface::PythonDictionaryEntry;
        FStateOutputInterface::fDataOutType = FStateOutputInterface::PhiAndDPhiDt;

        CustomStringSeparatedSOF formatter(", ");
        file << "# {" << InterfaceManager::getInstance().renderAsPythonDictionaryEnrties() /*<< ", " */ << "\"t\": " << t
             << ", \"phi\": (" << formatter(*spaceData.first) << ")"
             << ", \"dPhiDt\": (" << formatter(*spaceData.second) << ")}";

        file.flush();
        file.close();
    } catch (std::system_error& e) {
        std::cout << "Error: " << e.code().message() << "." << std::endl;
        file.close();
    }
}

bool OutputSnapshot::shouldOutput(const double, const long unsigned timeStep) {
    for(const size_t step : snapSteps)
        if(step == timeStep) return true;

    return false;
}

size_t OutputSnapshot::computeNextRecStep() {
    size_t smallest = snapSteps.back();
    for(auto step : snapSteps) if(step < smallest) smallest = step;

    return smallest;
}

String OutputSnapshot::description() const {
    return String("snapshots of arbitrary time");
}

bool OutputSnapshot::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) {
    std::cout << "The very last step: " << theVeryLastOutputInformation.getSteps() << std::endl;
    return true;
}

