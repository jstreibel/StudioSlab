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
    typedef std::function<DevFloat(DevFloat)> ScalingFunction;

    enum ColorBarMode {
        AllFieldValues=0,
        ValuesInSatRangeOnly=1
    };

    struct ColorBarParams {
        DevFloat kappa;
        DevFloat phi_sat;
        DevFloat phi_max;
        DevFloat phi_min;
        bool symmetric;
        ColorBarMode mode;
    };

    class ColorBarArtist final : public FArtist {
        VertexBuffer vertexBuffer;
        FShader shader;
        CMapTexturePtr texture;
        bool textureDirty = true;
        RectI rect;

        bool autoColorBarTop = true;
        int general_slack = 15;
        int right_slack = 0;
        int left = -300;
        int cbarWidth_pixels = -0.35 * left;
        float cbarHeight_clamp = 0.96;
        int cbarTop_pixels = 10;

        int n_annotations = 9;
        int decimal_places = 5;
        bool scientific_notation = false;

        ColorBarParams params;

        ScalingFunction scalingFunction;
        ScalingFunction inverseScalingFunction;

        Pointer<const ColorMap> colorMap;

        void updateTexture();
        void setLocation(RectI loc);
    public:
        explicit ColorBarArtist(RectI loc={50,150, 50, 750});

        void setColorMap(const Pointer<const ColorMap>& colorMap);
        auto getTexture() -> CMapTexturePtr;

        bool HasGUI() override;

        void DrawGUI() override;

        void setKappa(DevFloat);
        void setPhiSaturation(DevFloat);
        void setSymmetric(bool);
        void setPhiMax(DevFloat);
        void setPhiMin(DevFloat);
        void setMode(ColorBarMode);

        CountType getSamples() const;

        bool Draw(const FPlot2DWindow &) override;

        void setScalingFunction(std::function<DevFloat(DevFloat)>);
        void setInverseScalingFunction(std::function<DevFloat(DevFloat)>);
    };

} // OpenGL

#endif //STUDIOSLAB_COLORBARARTIST_H
