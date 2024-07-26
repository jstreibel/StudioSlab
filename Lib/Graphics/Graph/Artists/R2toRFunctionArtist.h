//
// Created by joao on 2/10/23.
//

#ifndef STUDIOSLAB_R2TORFUNCTIONARTIST_H
#define STUDIOSLAB_R2TORFUNCTIONARTIST_H

#include "Graphics/Graph/Artists/Artist.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Texture1D_Color.h"
#include "Graphics/OpenGL/Artists/ColorBarArtist.h"
#include "Graphics/Styles/ColorMap.h"
#include "Graphics/OpenGL/Texture2D_Real.h"

#include "Math/Function/R2toR/Model/R2toRFunction.h"
#include "Math/Constants.h"
#include "Graphics/Graph/Util/FieldTextureKontraption.h"
#include "Graphics/Graph/Artists/Painters/Painter.h"
#include "Graphics/Graph/Artists/Painters/Colormap1DPainter.h"

namespace Slab::Graphics {

    using namespace Math;

    class R2toRFunctionArtist : public Artist {
        enum Scale {
            Linear,
            Log,
            Logit,
            LogEpsilon
        };

        bool validTextureData = false;

        Unit funcUnit;

        Pointer<FieldTextureKontraption> textureKontraptions;
        bool anti_alias=false;

        Pointer<R2toRPainter> painter;
        R2toR::Function_constptr func;

        void invalidateTextureData();
        void repopulateTextureBuffer();

        void drawGUI() override;

    public:
        explicit R2toRFunctionArtist();

        bool draw(const PlottingWindow &d) override;

        bool hasGUI() override;

        void setFunction(R2toR::Function_constptr function, const Unit& unit=Constants::One);
        auto getFunction() const -> R2toR::Function_constptr;

        void setPainter(Pointer<R2toRPainter> program);
        auto getPainter() -> Pointer<R2toRPainter>;

        void updateMinMax(bool force=false);

        void set_xPeriodicOn();

        auto getXHairInfo(const Point2D &XHairCoord) const -> Str override;

        auto getFieldTextureKontraption() const -> Pointer<FieldTextureKontraption>;

        void setLabel(Str label) override;
    };

    DefinePointers(R2toRFunctionArtist)

} // Graphics

#endif //STUDIOSLAB_R2TORFUNCTIONARTIST_H
