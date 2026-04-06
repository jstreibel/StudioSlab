#include "ScalarTimeDFTListenerV2.h"

#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "Math/Function/RtoR/Operations/DiscreteFourierTransform.h"

namespace Slab::Math::Numerics::V2 {

    FScalarTimeDFTListenerV2::FScalarTimeDFTListenerV2(FExtractor extractor, Str name)
    : Extractor(std::move(extractor))
    , Name(std::move(name)) {
        if (!Extractor) throw Exception("FScalarTimeDFTListenerV2 requires an extractor.");
    }

    auto FScalarTimeDFTListenerV2::Collect(const FSimulationEventV2 &event) -> void {
        const auto valueOpt = Extractor(event);
        if (!valueOpt.has_value()) return;

        const auto t = event.Cursor.SimulationTime.has_value()
            ? *event.Cursor.SimulationTime
            : static_cast<DevFloat>(event.Cursor.Step);

        SampleTimes.push_back(t);
        SampleValues.push_back(*valueOpt);
    }

    auto FScalarTimeDFTListenerV2::RecomputeDFT() -> void {
        Magnitudes = nullptr;
        if (SampleValues.size() < 2) return;

        const auto t0 = SampleTimes.front();
        const auto t1 = SampleTimes.back();
        if (Common::AreEqual(t0, t1)) return;

        auto numeric = New<Math::RtoR::NumericFunction_CPU>(RealArray(SampleValues.data(), SampleValues.size()), t0, t1);
        const auto dft = Math::RtoR::FDFT::Compute(*numeric);
        Magnitudes = Math::RtoR::FDFT::Magnitudes(dft);
    }

    auto FScalarTimeDFTListenerV2::OnRunStarted(const FSimulationEventV2 &initialEvent) -> void {
        (void) initialEvent;
    }

    auto FScalarTimeDFTListenerV2::OnSample(const FSimulationEventV2 &event) -> void {
        Collect(event);
    }

    auto FScalarTimeDFTListenerV2::OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool {
        (void) finalEvent;
        RecomputeDFT();
        return true;
    }

    auto FScalarTimeDFTListenerV2::GetName() const -> Str {
        return Name;
    }

    auto FScalarTimeDFTListenerV2::GetSampleCount() const -> size_t {
        return SampleValues.size();
    }

    auto FScalarTimeDFTListenerV2::GetDFTMagnitudes() const -> TPointer<Math::RtoR::NumericFunction> {
        return Magnitudes;
    }

    auto FScalarTimeDFTListenerV2::GetSampleTimes() const -> const Vector<DevFloat> & {
        return SampleTimes;
    }

    auto FScalarTimeDFTListenerV2::GetSampleValues() const -> const Vector<DevFloat> & {
        return SampleValues;
    }

} // namespace Slab::Math::Numerics::V2
