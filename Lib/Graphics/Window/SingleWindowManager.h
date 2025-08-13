//
// Created by joao on 8/13/25.
//

#ifndef SINGLEWINDOWMANAGER_H
#define SINGLEWINDOWMANAGER_H
#include "WindowManager.h"

namespace Slab::Graphics {

    class FSingleWindowManager : public FWindowManager {
    public:
        void AddSlabWindow(const TPointer<FSlabWindow>&, bool hidden) override;

        bool NotifySystemWindowReshape(int w, int h) override;

    private:
        TPointer<FSlabWindow> SlabWindow;
        Resolution WidthSysWin = 200, HeightSysWin = 200;

    };

} // Graphics::Slab

#endif //SINGLEWINDOWMANAGER_H
