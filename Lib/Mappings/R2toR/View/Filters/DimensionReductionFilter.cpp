//

#include "DimensionReductionFilter.h"

#include "Phys/Space/Impl/DiscreteSpaceCPU.h"

#include "Mappings/R2toR/Model/R2toRDiscreteFunction.h"
#include "Models/KleinGordon/R2toR/EquationState.h"
#include "Base/Tools/Log.h"


//
// Created by joao on 15/10/2019.
DimensionReductionFilter::DimensionReductionFilter(PosInt resolution, RtoR2::StraightLine alongLine)
    : line(alongLine), N_low(resolution) {
    Log::Info("DimensionReductionFilter") << " will interpolate straight line from " << alongLine.getx0()
    << " to " << (alongLine.getx0() + alongLine.getr()) << Log::Flush;
}

DiscreteSpacePair DimensionReductionFilter::operator()(const OutputPacket &outputInfo) {
    R2toR::DiscreteFunction &f = outputInfo.getEqStateData<R2toR::EquationState>()->getPhi();
    // const DiscreteSpace &dPhiSpace = *outputInfo.getSpaceData().second;

    if(f.getSpace().getDim().getNDim() != 2) throw "Is rong.";

    f.getSpace().syncHost();

    const R2toR::Domain domain = f.getDomain();
    const auto L = domain.getLx();
    const Real h_low = L / N_low;
    const auto delta_s = line.getΔs();
    const auto sMin = line.get_sMin();
    DiscreteSpaceCPU *newPhi = new DiscreteSpaceCPU(getOutputDim(), h_low);
    auto &data = newPhi->getHostData();

    if(0) {
        const auto eps = 0.05;
        auto is = outputInfo.getSimTime() > L / 2 - eps && outputInfo.getSimTime() < L / 2 + eps;
        if (is) {
            Log::Debug("@t=") << outputInfo.getSimTime() << Log::Flush;

            auto N_ = 32;
            auto dx = L / N_;

            auto c = 0;
            for (Real y = -L / 2; y < L / 2; y += dx) {
                std::cout << std::endl << ++c << (c < 10 ? "   " : "  ");
                for (Real x = -L / 2; x < L / 2; x += dx) {
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

    for(PosInt i=0; i < N_low; i++){
        const Real s = sMin + delta_s * Real(i) / N_low; // parametro da reta.
        const auto x = line(s);

        data[i] = f(x);
    }

    return {newPhi, nullptr};
}

DimensionMetaData DimensionReductionFilter::getOutputDim() const {
    return DimensionMetaData({N_low});
}
