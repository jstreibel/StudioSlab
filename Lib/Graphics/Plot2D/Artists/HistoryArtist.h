//
// Created by joao on 21/05/24.
//

#ifndef STUDIOSLAB_HISTORYARTIST_H
#define STUDIOSLAB_HISTORYARTIST_H

#include "R2toRFunctionArtist.h"
#include "Graphics/Plot2D/Util/HistoryTexture2DUploadHelper.h"

namespace Slab::Graphics {

    class HistoryArtist : public R2toRFunctionArtist {
        HistoryTexture2DUploadHelper_ptr helper;
    public:
        explicit HistoryArtist() = default;

        void set_t(Real);
    };

    DefinePointers(HistoryArtist)

} // Graphics

#endif //STUDIOSLAB_HISTORYARTIST_H
