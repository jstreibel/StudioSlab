//
// Created by joao on 4/8/23.
//

#ifndef STUDIOSLAB_FLATFIELDDISPLAY_H
#define STUDIOSLAB_FLATFIELDDISPLAY_H

#include <utility>

#include "Core/Graphics/Styles/ColorMap.h"

#include "Math/Graph/Graph.h"

#include "Maps/R2toR/Model/R2toRFunction.h"
#include "Core/Graphics/OpenGL/Texture2D_Color.h"
#include "Core/Graphics/OpenGL/Texture2D_Real.h"
#include "Core/Graphics/OpenGL/Texture1D_Color.h"
#include "Core/Graphics/OpenGL/Artists/ColorBar.h"
#include "Math/Constants.h"


namespace R2toR::Graphics {
    class FlatFieldDisplay : public Core::Graphics::Graph2D {
        enum Scale {
            Linear,
            Log,
            Logit,
            LogEpsilon
        };

        bool validTextureData = false;

        Styles::ColorMap cMap           = Styles::ColorMaps["BrBG"];
        OpenGL::Texture1D_Color* cMap_texture = nullptr;
        OpenGL::ColorBar colorBar;
        bool logScale                   = true;
        Real cMap_epsArg                =  1;
        Real cMap_min                   = -1.1;
        Real cMap_max                   =  1.1;
        bool symmetricMaxMin            = true;

        OpenGL::VertexBuffer vertexBuffer;
        OpenGL::Shader program;

        void drawFlatField();

        void computeColormapTexture();
    protected:
        Unit funcUnit;
        R2toR::Function::ConstPtr func    = nullptr;
        OpenGL::Texture2D_Real*   textureData = nullptr;

        void computeGraphRanges();

        Str getXHairLabel(const Point2D &coords) override;

        void drawGUI() override;

        void invalidateTextureData();
        void repopulateTextureBuffer();

    public:
        explicit FlatFieldDisplay(Str title="Full 2D", Real phiMin=-1., Real phiMax=1.);

        void setFunction(R2toR::Function::ConstPtr function, Unit unit=Constants::One);

        auto getFunction() const -> R2toR::Function::ConstPtr;

        void setColorMap(const Styles::ColorMap& colorMap);

        void set_xPeriodicOn();

        void draw() override;

        bool notifyMouseWheel(double dx, double dy) override;

        void notifyReshape(int newWinW, int newWinH) override;
    };
}

#endif //STUDIOSLAB_FLATFIELDDISPLAY_H
