//
// Created by joao on 7/1/24.
//

#include "AnalysisMonitor.h"

namespace Studios::Fields {

    void AnalysisMonitor::setDataView(int index) {
        if (index > dataViews.size() - 1) return;

        auto oldDataView = currentDataView;
        auto newDataView = dataViews[index];

        if (newDataView == oldDataView) return;

        removeWindow(currentDataView);

        currentDataView = newDataView;

        addWindow(currentDataView, true, -1);

        arrangeWindows();

        newDataView->notifyBecameVisible();
        if (oldDataView != nullptr) oldDataView->notifyBecameInvisible();
    }

    void AnalysisMonitor::draw() { WindowPanel::draw(); }

    void AnalysisMonitor::addDataView(const Slab::Pointer<DataView> &dataView) {
        dataViews.push_back(dataView);
    }

    bool
    AnalysisMonitor::notifyKeyboard(Slab::Core::KeyMap key, Slab::Core::KeyState state, Slab::Core::ModKeys modKeys) {
        if(state==Slab::Core::Release && modKeys.Mod_Alt) {
            int n = key - Slab::Core::Key_1 + 1;

            setDataView(n);

            return true;
        }

        return WindowPanel::notifyKeyboard(key, state, modKeys);
    }

} // Studios::Fields