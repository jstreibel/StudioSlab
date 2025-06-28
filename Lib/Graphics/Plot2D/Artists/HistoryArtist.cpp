//
// Created by joao on 21/05/24.
//

#include "HistoryArtist.h"

namespace Slab::Graphics {

    void HistoryArtist::set_t(DevFloat t) {
        auto func = this->getFunction();

        if(func == nullptr) return;

        if(helper == nullptr){
            auto &discreteFunc = dynamic_cast<const R2toR::NumericFunction&>(*func);

            auto textureKontraption = getFieldTextureKontraption();

            helper = Slab::New<HistoryTexture2DUploadHelper>(Slab::Naked(discreteFunc), textureKontraption);
        }

        helper->uploadUpTo(t);
    }
} // Graphics