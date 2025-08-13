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

    class R2toRFunctionArtist : public FArtist {
        enum Scale {
            Linear,
            Log,
            Logit,
            LogEpsilon
        };

        bool validTextureData = false;
        bool dataIsMutable = false;

        Unit funcUnit;

        TPointer<FieldTextureKontraption> textureKontraptions;
        bool anti_alias=false;

        Map<Str, TPointer<R2toRPainter>> painters;
        TPointer<R2toRPainter> current_painter;
        R2toR::Function_constptr func;

        void invalidateTextureData();
        void repopulateTextureBuffer();

        void DrawGUI() override;

    public:
        explicit R2toRFunctionArtist();

        bool Draw(const FPlot2DWindow &) override;

        bool HasGUI() override;

        void setFunction(R2toR::Function_constptr, const Unit& unit=Constants::One);
        auto getFunction() const -> R2toR::Function_constptr;

        void setDataMutable(bool);

        void setPainter(TPointer<R2toRPainter>);
        auto getPainter() -> TPointer<R2toRPainter>;
        auto getPainter(const Str &name) -> TPointer<R2toRPainter>;

        void updateMinMax(bool force=false) const;

        void set_xPeriodicOn() const;

        auto GetXHairInfo(const Point2D &XHairCoord) const -> Str override;

        auto getFieldTextureKontraption() const -> TPointer<FieldTextureKontraption>;

        void SetLabel(Str label) override;
    };

    DefinePointers(R2toRFunctionArtist)

} // Graphics

#endif //STUDIOSLAB_R2TORFUNCTIONARTIST_H
