//
// Created by joao on 7/22/19.
//

#include "OutputSnapshots.h"

#include "Math/Numerics/ODE/Output/Format/CustomStringSeparatedSOF.h"
#include "Math/Numerics/OutputChannel.h"
#include "Core/Tools/Log.h"

#include "../../../../../Core/Controller/InterfaceManager.h"

#include <fstream>
#include <iomanip>
#include <utility>


namespace Slab::Math {

    using Core::Log;

    OutputSnapshot::OutputSnapshot(Str customFileDescription,
                                   const size_t T_fileNamePrecision)
            : FOutputChannel("Snapshot output", 1),
              customFileDescription(std::move(customFileDescription)),
              T_fileNamePrecision(T_fileNamePrecision) {}

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

        if (customFileDescription != "")
            filePhiNameStream << customFileDescription;
        else
            filePhiNameStream << "./snapshot-step=" << outInfo.GetSteps();
        filePhiNameStream << ".oscs";

        Str fileName = filePhiNameStream.str();
        auto &log = Log::Info();
        log.setf(std::ios::fixed, std::ios::floatfield);
        log.precision(4);

        log << "Saving snapshot for step = " << outInfo.GetSteps() << " in \'" << fileName << "\'... " << Log::Flush;
        {
            std::ofstream file;
            file.exceptions(std::ofstream::failbit | std::ofstream::badbit);

            try {
                file.open(fileName);
                Log::Note() << "Opened file \"" << fileName << "\"" << Log::Flush;

                EqStateOutputInterface::format = EqStateOutputInterface::PythonDictionaryEntry;
                EqStateOutputInterface::fDataOutType = EqStateOutputInterface::PhiAndDPhiDt;

                /*
                CustomStringSeparatedSOF formatter(", ");
                file << "# {" << InterfaceManager::getInstance().renderAsPythonDictionaryEntries() *//*<< ", " *//* << "\"t\": "
                     << t
                     << ", \"phi\": (" << formatter(*spaceData.first) << ")"
                     << ", \"dPhiDt\": (" << formatter(*spaceData.second) << ")}";
                */

                // _outputToFile(outInfo.getSpaceData(), t, fileName);
                _outputToFile(file);

                file.flush();
            } catch (std::system_error &e) {
                Log::Error() << "File not saved. std::system_error::code().message(): "
                                "\"" << e.code().message() << "\"." << Log::Flush;
            }

            file.close();
            Log::Note() << "Closed file \"" << fileName << "\"" << Log::Flush;

        }
        Log::Success() << "Snapshot saved! File '" << fileName << "'" << Log::Flush;
    }

    bool OutputSnapshot::ShouldOutput(const long unsigned timeStep) {
        for (const size_t step: snapSteps)
            if (step == timeStep) return true;

        return false;
    }

    size_t OutputSnapshot::ComputeNextRecStep(UInt currStep) {
        size_t smallest = snapSteps.back();
        for (auto step: snapSteps) if (step < smallest) smallest = step;

        return smallest;
    }


}