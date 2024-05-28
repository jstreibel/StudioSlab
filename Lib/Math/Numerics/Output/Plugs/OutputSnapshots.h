#ifndef OUTPUTSNAPSHOTS1D_H
#define OUTPUTSNAPSHOTS1D_H

#include "Math/Numerics/Output/Plugs/Socket.h"


namespace Slab::Math {

    class OutputSnapshot : public Socket {

    private:
        Vector<size_t> snapSteps;
        Str customFileDescription;

        size_t T_fileNamePrecision;
    public:
        explicit OutputSnapshot(const NumericConfig &params, const Str &customFileDescription = "",
                       size_t fileNamePrecision = 4);

        ~OutputSnapshot() override;

        void addSnapshotStep(size_t snapshotStep);

        void doOutput(const OutputPacket &outInfo, const Str &customFileDescription = "",
                             size_t T_fileNamePrecision = 4);

        size_t computeNextRecStep(UInt currStep) override;

    protected:
        virtual void _outputToFile(std::ofstream &file) = 0;

    protected:
        bool shouldOutput(Real t, long unsigned timeStep) override;

        void handleOutput(const OutputPacket &outInfo) override {
            OutputSnapshot::doOutput(outInfo, customFileDescription, 4);
        }
    };


}

#endif // OUTPUTSNAPSHOTS1D_H
