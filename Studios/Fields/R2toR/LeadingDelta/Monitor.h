//
// Created by joao on 09/06/23.
//

#ifndef STUDIOSLAB_MONITOR_H
#define STUDIOSLAB_MONITOR_H

#include "Models/KleinGordon/R2toR/Graphics/R2toROutputOpenGLGeneric.h"



namespace Studios::Fields::R2toRLeadingDelta {

    using namespace Slab;
    using namespace Math;

    class OutGL : public Models::KGR2toR::OutputOpenGL {

        Pointer<R2toR::Function> drivingFunction;

        Pointer<PointSet> totalEnergyData;
        Graphics::FPlot2DWindow mTotalEnergyGraph;

        Pointer<PointSet> numericEnergyData;
        Pointer<PointSet> analyticEnergyData;
        Graphics::FPlot2DWindow mEnergyGraph;

        Pointer<PointSet> energyRatioData;
        Graphics::FPlot2DWindow mEnergyRatioGraph;

        Graphics::FPlot2DWindow mSpeedsGraph;
        Graphics::FPlot2DWindow mEnergyDensityGraph;

    public:

        OutGL(CountType max_steps, Pointer<R2toR::Function> drivingFunction);

        auto ImmediateDraw() -> void override;

    };
}


#endif //STUDIOSLAB_MONITOR_H
