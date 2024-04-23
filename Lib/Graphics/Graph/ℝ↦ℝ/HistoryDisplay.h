//
// Created by joao on 18/08/23.
//

#ifndef STUDIOSLAB_HISTORYDISPLAY_H
#define STUDIOSLAB_HISTORYDISPLAY_H


#include "Graphics/Graph/ℝ²↦ℝ/FlatFieldDisplay.h"
#include "HistoryTexture2DUploadHelper.h"

namespace Graphics {

    class HistoryDisplay : public Graphics::FlatFieldDisplay {
        // Count nextRow = 0;
        // Real lastUpdatedTime = -1;

        std::shared_ptr<HistoryTexture2DUploadHelper> helper;

    public:
        explicit HistoryDisplay(Str name="Full history");

        void set_t(Real t);
    };

}


#endif //STUDIOSLAB_HISTORYDISPLAY_H
