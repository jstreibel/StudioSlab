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

    class ColorBarArtist final : public Artist {
        VertexBuffer vertexBuffer;
        Shader shader;
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

        bool hasGUI() override;

        void drawGUI() override;

        void setKappa(Real);
        void setPhiSaturation(Real);
        void setSymmetric(bool);
        void setPhiMax(Real);
        void setPhiMin(Real);
        void setMode(ColorBarMode);

        Count getSamples() const;

        bool draw(const Plot2DWindow &) override;

        void setScalingFunction(std::function<Real(Real)>);
        void setInverseScalingFunction(std::function<Real(Real)>);
    };

} // OpenGL

#endif //STUDIOSLAB_COLORBARARTIST_H
