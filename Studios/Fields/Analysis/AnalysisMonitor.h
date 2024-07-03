//
// Created by joao on 7/1/24.
//

#ifndef STUDIOSLAB_ANALYSISMONITOR_H
#define STUDIOSLAB_ANALYSISMONITOR_H

#include "Graphics/BaseMonitor.h"
#include "DataView.h"

namespace Studios::Fields {

    class AnalysisMonitor : public Slab::Graphics::WindowPanel {
        Slab::Vector<Slab::Pointer<DataView>> dataViews;
        Slab::Pointer<DataView> currentDataView;

        void setDataView(int index);

    public:
        void draw() override;

        void addDataView(const Slab::Pointer<DataView>&);

        bool notifyKeyboard(Slab::Core::KeyMap key, Slab::Core::KeyState state, Slab::Core::ModKeys modKeys) override;
    };

} // Studios::Fields

#endif //STUDIOSLAB_ANALYSISMONITOR_H
