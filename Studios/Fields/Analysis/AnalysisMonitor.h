//
// Created by joao on 7/1/24.
//

#ifndef STUDIOSLAB_ANALYSISMONITOR_H
#define STUDIOSLAB_ANALYSISMONITOR_H

#include "Graphics/BaseMonitor.h"
#include "DataView.h"

namespace Studios::Fields {

    class FAnalysisMonitor : public Slab::Graphics::WindowPanel {
        Slab::Vector<Slab::Pointer<FDataView>> dataViews;
        Slab::Pointer<FDataView> currentDataView;

        void setDataView(int index);

    public:
        void draw() override;

        void addDataView(const Slab::Pointer<FDataView>&);

        bool notifyKeyboard(Slab::KeyMap key, Slab::KeyState state, Slab::ModKeys modKeys) override;
    };

    using AnalysisMonitor [[deprecated("Use FAnalysisMonitor")]] = FAnalysisMonitor;

} // Studios::Fields

#endif //STUDIOSLAB_ANALYSISMONITOR_H
