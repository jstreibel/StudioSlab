//
// Created by joao on 4/6/23.
//

#ifndef STUDIOSLAB_OUTGL_H
#define STUDIOSLAB_OUTGL_H

#include "Mappings/RtoR/View/Graphics/RtoRMonitor.h"

namespace RtoR {

    namespace Signal {

        class JackControl : public Window {
        public:
            void draw(bool decorated, bool clear) const override;
        };

        class OutGL : public RtoR::Monitor {
            Window *fieldWindow;

            Window *signalBufferWindow;
            GraphRtoR signalBufferGraph;
            Window *signalFullWindow;
            GraphRtoR signalFullGraph;
            JackControl jackControlWindow;

            GraphRtoR fullRecordingGraph;
            Window fullRecordingWindow;

            bool gotNewData = true;

            std::vector<std::vector<Real>> getHistoryMatrixData();
            std::vector<RtoR::ArbitraryFunction*> history;
        protected:

            std::vector <Real> probingData;

            void _out(const OutputPacket &outInfo"") override;

        public:
            OutGL(Real xMin, Real xMax, Real phiMin, Real phiMax);

            void draw() override;

            auto getWindowSizeHint() -> IntPair override;

            bool notifyKeyboard(unsigned char key, int x, int y) override;
        };
    }
}


#endif //STUDIOSLAB_OUTGL_H
