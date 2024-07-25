//
// Created by joao on 7/25/24.
//

#ifndef STUDIOSLAB_COLORMAP1DPAINTER_H
#define STUDIOSLAB_COLORMAP1DPAINTER_H

#include "Painter.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Graphics/Styles/ColorMap.h"

namespace Slab::Graphics {

    class Colormap1DPainter : public Painter {
        Pointer<Math::R2toR::NumericFunction> field_data;
        Reference<ColorMap> colorMap;
        float  kappa            = 1; //  contrast
        float  saturation_value = 1.1;
        float  eps_offset       = 0.0f;
        bool   symmetric_maxmin = true;

        void adjustScale();
        void updateColorbar();
    public:
        Colormap1DPainter();

        void setFieldData(Pointer<Math::R2toR::NumericFunction>);
        void setColorMap(Reference<ColorMap>);

        void drawGUI() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_COLORMAP1DPAINTER_H
