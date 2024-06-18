//
// Created by joao on 24/09/23.
//

#ifndef STUDIOSLAB_COLORBARARTIST_H
#define STUDIOSLAB_COLORBARARTIST_H

#include "Overlay.h"
#include "Graphics/Styles/ColorMap.h"
#include "Graphics/OpenGL/Texture1D_Color.h"

namespace Slab::Graphics::OpenGL {
    typedef std::shared_ptr<Graphics::OpenGL::Texture1D_Color> CMapTexturePtr;
    typedef std::function<Real(Real)> ScalingFunction;

    enum ColorBarMode {
        AllFieldValues=0,
        ValuesInSatRangeOnly=1
    };

    struct ColorBarParams {
        Real kappa;
        Real phi_sat;
        Real phi_max;
        Real phi_min;
        bool symmetric;
        ColorBarMode mode;
    };

    class ColorBarArtist : public Artist {
        VertexBuffer vertexBuffer;
        Shader shader;
        CMapTexturePtr texture;
        bool textureDirty = true;
        RectI rect;

        ColorBarParams params;

        ScalingFunction scalingFunction;
        ScalingFunction inverseScalingFunction;

        ColorMap colorMap;

        void updateTexture(int samples=1024);
    public:
        explicit ColorBarArtist(RectI loc={50,150, 50, 750});

        void setLocation(RectI loc);
        void setColorMap(const ColorMap& colorMap);
        auto getTexture() -> CMapTexturePtr;

        void setKappa(Real);
        void setPhiSaturation(Real);
        void setSymmetric(bool);
        void setPhiMax(Real);
        void setPhiMin(Real);
        void setMode(ColorBarMode);

        bool draw(const PlottingWindow &) override;

        void setScalingFunction(std::function<Real(Real)>);
        void setInverseScalingFunction(std::function<Real(Real)>);
    };

} // OpenGL

#endif //STUDIOSLAB_COLORBARARTIST_H
