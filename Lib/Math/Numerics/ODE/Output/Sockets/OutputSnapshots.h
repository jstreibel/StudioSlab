#ifndef OUTPUTSNAPSHOTS1D_H
#define OUTPUTSNAPSHOTS1D_H

#include "Math/Numerics/OutputChannel.h"


namespace Slab::Math {

    class FOutputSnapshot : public FOutputChannel {

    private:
        Vector<size_t> snapSteps;
        Str customFileDescription;

        size_t T_fileNamePrecision;
    public:
        explicit FOutputSnapshot(Str customFileDescription = "",
                                 size_t fileNamePrecision = 4);

        ~FOutputSnapshot() override;

        void addSnapshotStep(size_t snapshotStep);

        void doOutput(const FOutputPacket &outInfo, const Str &customFileDescription = "",
                             size_t T_fileNamePrecision = 4);

        size_t ComputeNextRecStep(UInt currStep) override;

    protected:
        virtual void _outputToFile(std::ofstream &file) = 0;

    protected:
        bool ShouldOutput(long unsigned timeStep) override;

        void HandleOutput(const FOutputPacket &outInfo) override {
            FOutputSnapshot::doOutput(outInfo, customFileDescription, 4);
        }
    };

    using OutputSnapshot [[deprecated("Use FOutputSnapshot")]] = FOutputSnapshot;


}

#endif // OUTPUTSNAPSHOTS1D_H
