//
// Created by joao on 2/10/23.
//

#ifndef STUDIOSLAB_R2TORFUNCTIONARTIST_H
#define STUDIOSLAB_R2TORFUNCTIONARTIST_H

#include "Graphics/Plot2D/Artists/Artist.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Texture1D_Color.h"
#include "Graphics/Styles/ColorMap.h"

#include "Math/Function/R2toR/Model/R2toRFunction.h"
#include "Math/Constants.h"
#include "Graphics/Plot2D/Util/FieldTextureKontraption.h"
#include "Graphics/Plot2D/Artists/Painters/Colormap1DPainter.h"
#include "Utils/Map.h"

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
        bool dataIsMutable = false;

        Unit funcUnit;

        Pointer<FieldTextureKontraption> textureKontraptions;
        bool anti_alias=false;

        Map<Str, Pointer<R2toRPainter>> painters;
        Pointer<R2toRPainter> current_painter;
        R2toR::Function_constptr func;

        void invalidateTextureData();
        void repopulateTextureBuffer();

        void drawGUI() override;

    public:
        explicit R2toRFunctionArtist();

        bool draw(const Plot2DWindow &) override;

        bool hasGUI() override;

        void setFunction(R2toR::Function_constptr, const Unit& unit=Constants::One);
        auto getFunction() const -> R2toR::Function_constptr;

        void setDataMutable(bool);

        void setPainter(Pointer<R2toRPainter>);
        auto getPainter() -> Pointer<R2toRPainter>;
        auto getPainter(const Str &name) -> Pointer<R2toRPainter>;

        void updateMinMax(bool force=false) const;

        void set_xPeriodicOn() const;

        auto getXHairInfo(const Point2D &XHairCoord) const -> Str override;

        auto getFieldTextureKontraption() const -> Pointer<FieldTextureKontraption>;

        void setLabel(Str label) override;
    };

    DefinePointers(R2toRFunctionArtist)

} // Graphics

#endif //STUDIOSLAB_R2TORFUNCTIONARTIST_H
