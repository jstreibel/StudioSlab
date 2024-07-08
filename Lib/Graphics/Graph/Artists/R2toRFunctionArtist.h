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

        Pointer<ColorMap> cMap           = ColorMaps["BrBG"]->clone();
        Real cMap_kappaArg                =  1; // contrast
        Real cMap_saturationValue       =  1.1;
        float eps                       = 0.0f;
        bool symmetricMaxMin            = true;

        bool showColorBar = true;
        Pointer<Graphics::OpenGL::ColorBarArtist> colorBar;

        Pointer<FieldTextureKontraption> textureKontraptions;
        bool anti_alias=false;

        Pointer<OpenGL::Shader> program;
        R2toR::Function_constptr func;

        bool dirty_minmax = true;
        Real field_min = 0.0;
        Real field_max = 1.0;

        void updateColorBar();
        void invalidateTextureData();
        void repopulateTextureBuffer();

        void drawGUI() override;

    public:
        explicit R2toRFunctionArtist();

        bool draw(const PlottingWindow &d) override;

        bool hasGUI() override;

        void setFunction(R2toR::Function_constptr function, const Unit& unit=Constants::One);
        auto getFunction() const -> R2toR::Function_constptr;

        void setProgram(Pointer<OpenGL::Shader> program);
        auto getProgram() -> Pointer<OpenGL::Shader>;

        void updateMinMax();
        void flagMinMaxAsDirty();

        void setColorMap(const Pointer<ColorMap>& colorMap);
        auto getColorBarArtist() const -> Pointer<Graphics::OpenGL::ColorBarArtist>;

        void set_xPeriodicOn();

        auto getXHairInfo(const Point2D &XHairCoord) const -> Str override;

        void adjustScale();

        auto getFieldTextureKontraption() const -> Pointer<FieldTextureKontraption>;

        void setLabel(Str label) override;
    };

    DefinePointers(R2toRFunctionArtist)

} // Graphics

#endif //STUDIOSLAB_R2TORFUNCTIONARTIST_H
