//
// Created by joao on 21/05/24.
//

#include "HistoryArtist.h"

namespace Slab::Graphics {

    void HistoryArtist::set_t(Real t) {
        auto func = this->getFunction();

        if(func == nullptr) return;

        if(helper == nullptr){
            auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

            auto textureData = getFieldTextureData();

            helper = Slab::New<HistoryTexture2DUploadHelper>(Slab::DummyPointer(discreteFunc), textureData);
        }

        helper->uploadUpTo(t);
    }
} // Graphics