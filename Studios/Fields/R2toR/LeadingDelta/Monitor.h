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

        TPointer<R2toR::Function> drivingFunction;

        TPointer<PointSet> totalEnergyData;
        Graphics::FPlot2DWindow mTotalEnergyGraph;

        TPointer<PointSet> numericEnergyData;
        TPointer<PointSet> analyticEnergyData;
        Graphics::FPlot2DWindow mEnergyGraph;

        TPointer<PointSet> energyRatioData;
        Graphics::FPlot2DWindow mEnergyRatioGraph;

        Graphics::FPlot2DWindow mSpeedsGraph;
        Graphics::FPlot2DWindow mEnergyDensityGraph;

    public:

        OutGL(CountType max_steps, TPointer<R2toR::Function> drivingFunction);

        auto ImmediateDraw(const Graphics::FPlatformWindow&) -> void override;

    };
}


#endif //STUDIOSLAB_MONITOR_H
