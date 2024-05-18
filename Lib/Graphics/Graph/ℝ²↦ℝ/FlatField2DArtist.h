//
// Created by joao on 2/10/23.
//

#ifndef STUDIOSLAB_FLATFIELD2DARTIST_H
#define STUDIOSLAB_FLATFIELD2DARTIST_H

#include "Graphics/Graph/Artists/Artist.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Texture1D_Color.h"
#include "Graphics/OpenGL/Artists/ColorBarArtist.h"
#include "Graphics/Styles/ColorMap.h"
#include "Graphics/OpenGL/Texture2D_Real.h"

#include "Math/Function/R2toR/Model/R2toRFunction.h"
#include "Math/Constants.h"

namespace Graphics {

    class FlatField2DArtist : public Artist {
        enum Scale {
            Linear,
            Log,
            Logit,
            LogEpsilon
        };

        typedef std::shared_ptr<Graphics::OpenGL::Texture2D_Real> FieldDataTexturePtr;

        bool validTextureData = false;

        Unit funcUnit;

        Real field_min;
        Real field_max;

        Styles::ColorMap cMap           = Styles::ColorMaps["BrBG"];
        Real cMap_kappaArg                =  1; // contrast
        Real cMap_saturationValue       =  1.1;
        bool symmetricMaxMin            = true;

        bool showColorBar = true;
        Graphics::OpenGL::ColorBarArtist colorBar;

        Graphics::OpenGL::VertexBuffer vertexBuffer;
        Graphics::OpenGL::Shader program;
        R2toR::Function::ConstPtr func    = nullptr;
        FieldDataTexturePtr   textureData = nullptr;

        Str name;

        void updateColorBar();
        void invalidateTextureData();
        void repopulateTextureBuffer();

        void drawGUI();

    public:
        explicit FlatField2DArtist(Str name);

        void draw(const Graph2D &d) override;

        void setFunction(R2toR::Function::ConstPtr function, const Unit& unit=Constants::One);
        auto getFunction() const -> R2toR::Function::ConstPtr;
        void setColorMap(const Styles::ColorMap& colorMap);
        void set_xPeriodicOn();

        void adjustScale();

        auto getFieldTextureData() const -> FieldDataTexturePtr;


    };

} // Graphics

#endif //STUDIOSLAB_FLATFIELD2DARTIST_H
