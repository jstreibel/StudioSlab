//
// Created by joao on 15/10/2019.

#include "DimensionReductionFilter.h"

#include "Math/VectorSpace/Impl/DiscreteSpaceCPU.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Models/KleinGordon/R2toR/EquationState.h"
#include "Core/Tools/Log.h"


namespace Slab::Math::R2toR {

    DimensionReductionFilter::DimensionReductionFilter(UInt resolution, RtoR2::StraightLine alongLine, DevFloat L)
            : line(alongLine), N_low(resolution), L(L) {
        Core::Log::Info("DimensionReductionFilter") << " will interpolate straight line from " << alongLine.getx0()
                                              << " to " << (alongLine.getx0() + alongLine.getr()) << Core::Log::Flush;
    }

    DiscreteSpacePair DimensionReductionFilter::operator()(const OutputPacket &outputInfo) {
        IN kgState = *outputInfo.GetNakedStateData<R2toR::EquationState>();
        auto &f = dynamic_cast<R2toR::NumericFunction&>(kgState.getPhi());
        // const DiscreteSpace &dPhiSpace = *outputInfo.getSpaceData().second;

        if (f.getSpace().getMetaData().getNDim() != 2) throw "Is rong.";

        f.getSpace().syncHost();

        const auto delta_s = line.getΔs();
        const auto sMin = line.get_sMin();
        DiscreteSpaceCPU *newPhi = new DiscreteSpaceCPU(getOutputDim());
        auto &data = newPhi->getHostData();

        if (0) {
            const auto eps = 0.05;
            if (false) {
                auto N_ = 32;
                auto dx = L / N_;

                auto c = 0;
                for (DevFloat y = -L / 2; y < L / 2; y += dx) {
                    std::cout << std::endl << ++c << (c < 10 ? "   " : "  ");
                    for (DevFloat x = -L / 2; x < L / 2; x += dx) {
                        {
                            Real2D r = {x, y};
                            auto val = f(r);
                            Common::GetDensityChar(100 * f(r));
                            std::cout << val << " ";
                        }

                    }
                }
            }
        }

        for (UInt i = 0; i < N_low; i++) {
            const DevFloat s = sMin + delta_s * DevFloat(i) / N_low; // parametro da reta.
            const auto x = line(s);

            data[i] = f(x);
        }

        return {newPhi, nullptr};
    }

    DimensionMetaData DimensionReductionFilter::getOutputDim() const {
        const DevFloat h_low = L / N_low;
        return DimensionMetaData({N_low}, {h_low});
    }


}