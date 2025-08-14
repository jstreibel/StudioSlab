//
// Created by joao on 4/6/23.
//

#ifndef STUDIOSLAB_OUTGL_H
#define STUDIOSLAB_OUTGL_H

#include "Models/KleinGordon/RtoR/RtoRMonitor.h"

namespace Slab::Math::RtoR {

    namespace Signal {

        class JackControl : public Window {
        public:
            void draw() override;
        };

        class OutGL : public RtoR::Monitor {
            GraphRtoR signalBufferGraph;
            GraphRtoR signalFullGraph;
            JackControl jackControlWindow;

            GraphRtoR fullRecordingGraph;
            Window fullRecordingWindow;

            bool gotNewData = true;

            Vector<Vector<Real>> getHistoryMatrixData();
            Vector<RtoR::NumericFunction*> history;
        protected:

            Vector <Real> probingData;

            void handleOutput(const OutputPacket &outInfo) override;

        public:
            OutGL(const NumericConfig &params, DevFloat phiMin, DevFloat phiMax);

            void draw() override;

            auto getWindowSizeHint() -> IntPair override;

            bool notifyKeyboard(unsigned char key, int x, int y) override;
        };
    }
}


#endif //STUDIOSLAB_OUTGL_H
