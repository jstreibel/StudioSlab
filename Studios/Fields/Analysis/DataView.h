//
// Created by joao on 7/1/24.
//

#ifndef STUDIOSLAB_DATAVIEW_H
#define STUDIOSLAB_DATAVIEW_H

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

namespace Studios::Fields {

    class DataView : public Slab::Graphics::WindowPanel {
        using DataFunction = Slab::Math::Pointer<const R2toR::FNumericFunction>
;
        using DataArtist = Slab::Graphics::R2toRFunctionArtist_ptr;

        DataFunction function;
        DataArtist artist;

    public:
        DataView(DataFunction func=nullptr);

        virtual void setData(DataFunction func);

        virtual void notifyBecameVisible()   {};
        virtual void notifyBecameInvisible() {};
    };

} // Fields

#endif //STUDIOSLAB_DATAVIEW_H
