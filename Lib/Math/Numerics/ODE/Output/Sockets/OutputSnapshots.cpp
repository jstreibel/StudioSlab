//
// Created by joao on 7/22/19.
//

#include "OutputSnapshots.h"

#include "Math/Numerics/ODE/Output/Format/CustomStringSeparatedSOF.h"
#include "Math/Numerics/ODE/Output/Util/FieldStateOutputInterface.h"
#include "Math/Numerics/OutputChannel.h"
#include "Core/Tools/Log.h"

#include "../../../../../Core/Controller/InterfaceManager.h"

#include <fstream>
#include <iomanip>
#include <limits>
#include <utility>


namespace Slab::Math {

    using Core::FLog;

    FOutputSnapshot::FOutputSnapshot(Str customFileDescription,
                                     const size_t T_fileNamePrecision)
            : FOutputChannel("Snapshot output", 1),
              customFileDescription(std::move(customFileDescription)),
              T_fileNamePrecision(T_fileNamePrecision) {}

    FOutputSnapshot::~FOutputSnapshot() = default;

    void FOutputSnapshot::addSnapshotStep(const size_t snapshotStep) {
        snapSteps.push_back(snapshotStep);

        FLog::Attention() << "A snapshot will be taken at the step " << snapshotStep << FLog::Flush;
    }

    void FOutputSnapshot::doOutput(const FOutputPacket &outInfo, const Str &customFileDescription,
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
        auto &log = FLog::Info();
        log.setf(std::ios::fixed, std::ios::floatfield);
        log.precision(4);

        log << "Saving snapshot for step = " << outInfo.GetSteps() << " in \'" << fileName << "\'... " << FLog::Flush;
        {
            std::ofstream file;
            file.exceptions(std::ofstream::failbit | std::ofstream::badbit);

            try {
                file.open(fileName);
                FLog::Note() << "Opened file \"" << fileName << "\"" << FLog::Flush;

                FEqStateOutputInterface::format = FEqStateOutputInterface::PythonDictionaryEntry;
                FEqStateOutputInterface::fDataOutType = FEqStateOutputInterface::PhiAndDPhiDt;

                /*
                FCustomStringSeparatedSOF formatter(", ");
                file << "# {" << InterfaceManager::getInstance().renderAsPythonDictionaryEntries() *//*<< ", " *//* << "\"t\": "
                     << t
                     << ", \"phi\": (" << formatter(*spaceData.first) << ")"
                     << ", \"dPhiDt\": (" << formatter(*spaceData.second) << ")}";
                */

                // _outputToFile(outInfo.getSpaceData(), t, fileName);
                _outputToFile(file);

                file.flush();
            } catch (std::system_error &e) {
                FLog::Error() << "File not saved. std::system_error::code().message(): "
                                "\"" << e.code().message() << "\"." << FLog::Flush;
            }

            file.close();
            FLog::Note() << "Closed file \"" << fileName << "\"" << FLog::Flush;

        }
        FLog::Success() << "Snapshot saved! File '" << fileName << "'" << FLog::Flush;
    }

    bool FOutputSnapshot::ShouldOutput(const long unsigned timeStep) {
        for (const size_t step: snapSteps)
            if (step == timeStep) return true;

        return false;
    }

    size_t FOutputSnapshot::ComputeNextRecStep(UInt currStep) {
        size_t nextStep = std::numeric_limits<size_t>::max();

        for (const auto step : snapSteps) {
            if (step > currStep && step < nextStep) {
                nextStep = step;
            }
        }

        return nextStep;
    }


}
