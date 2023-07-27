//
// Created by joao on 09/06/23.
//

#ifndef STUDIOSLAB_MONITOR_H
#define STUDIOSLAB_MONITOR_H

#include "Phys/Graph/PointSetGraph.h"
#include "Mappings/R2toR/View/R2toROutputOpenGLGeneric.h"

namespace R2toR {

    namespace LeadingDelta {

        class OutGL : public R2toR::OutputOpenGL {
            R2toR::Function::Ptr drivingFunction;

            Spaces::PointSet::Ptr totalEnergyData;
            Phys::Graphing::PointSetGraph mTotalEnergyGraph;

            Spaces::PointSet::Ptr numericEnergyData;
            Spaces::PointSet::Ptr analyticEnergyData;
            Phys::Graphing::PointSetGraph mEnergyGraph;

            Spaces::PointSet::Ptr energyRatioData;
            Phys::Graphing::PointSetGraph mEnergyRatioGraph;

            GraphR2Section mSpeedsGraph;
            GraphR2Section mEnergyDensityGraph;


        public:
            OutGL(const NumericParams &params, R2toR::Function::Ptr drivingFunction, Real phiMin, Real phiMax);

            auto draw() -> void override;

            auto notifyKeyboard(unsigned char key, int x, int y) -> bool override;

            bool notifyMouseWheel(int wheel, int direction, int x, int y) override;
            bool notifyMousePassiveMotion(int x, int y) override;
            bool notifyMouseMotion(int x, int y) override;
            bool notifyMouseButton(int button, int dir, int x, int y) override;
        };
    }
}

#endif //STUDIOSLAB_MONITOR_H
