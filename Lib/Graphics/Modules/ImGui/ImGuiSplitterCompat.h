//
// Created by Codex on 2/27/26.
//

#ifndef STUDIOSLAB_IMGUISPLITTERCOMPAT_H
#define STUDIOSLAB_IMGUISPLITTERCOMPAT_H

#include "3rdParty/ImGui.h"

namespace Slab::Graphics {

    class FImGuiSplitterCompat {
    public:
        static bool Splitter(bool p_SplitVertically,
                             float p_Thickness,
                             float* p_Size1,
                             float* p_Size2,
                             float p_MinSize1,
                             float p_MinSize2,
                             float p_SplitterLongAxisSize = -1.0f);
    };

} // namespace Slab::Graphics

#endif // STUDIOSLAB_IMGUISPLITTERCOMPAT_H
