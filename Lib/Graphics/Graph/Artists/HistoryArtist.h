//
// Created by joao on 21/05/24.
//

#ifndef STUDIOSLAB_HISTORYARTIST_H
#define STUDIOSLAB_HISTORYARTIST_H

#include "R2toRFunctionArtist.h"
#include "HistoryTexture2DUploadHelper.h"

namespace Graphics {

    class HistoryArtist : public R2toRFunctionArtist {
        HistoryTexture2DUploadHelper_ptr helper;
    public:
        explicit HistoryArtist() = default;

        void set_t(Real);
    };

    DefinePointer(HistoryArtist)

} // Graphics

#endif //STUDIOSLAB_HISTORYARTIST_H
