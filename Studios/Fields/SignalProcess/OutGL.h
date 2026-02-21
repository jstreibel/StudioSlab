//
// Created by joao on 4/6/23.
//

#ifndef STUDIOSLAB_OUTGL_H
#define STUDIOSLAB_OUTGL_H

#include "Models/KleinGordon/RtoR/RtoRMonitor.h"

namespace Slab::Math::RtoR {

    namespace Signal {

        class FJackControl : public Window {
        public:
            void draw() override;
        };

        class FOutGL : public RtoR::Monitor {
            GraphRtoR signalBufferGraph;
            GraphRtoR signalFullGraph;
            FJackControl jackControlWindow;

            GraphRtoR fullRecordingGraph;
            Window fullRecordingWindow;

            bool gotNewData = true;

            Vector<Vector<Real>> getHistoryMatrixData();
            Vector<RtoR::NumericFunction*> history;
        protected:

            Vector <Real> probingData;

            void handleOutput(const OutputPacket &outInfo) override;

        public:
            FOutGL(const NumericConfig &params, DevFloat phiMin, DevFloat phiMax);

            void draw() override;

            auto getWindowSizeHint() -> IntPair override;

            bool notifyKeyboard(unsigned char key, int x, int y) override;
        };

        using JackControl [[deprecated("Use FJackControl")]] = FJackControl;
        using OutGL [[deprecated("Use FOutGL")]] = FOutGL;
    }
}


#endif //STUDIOSLAB_OUTGL_H
