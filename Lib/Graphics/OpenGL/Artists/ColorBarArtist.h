//
// Created by joao on 24/09/23.
//

#ifndef STUDIOSLAB_COLORBARARTIST_H
#define STUDIOSLAB_COLORBARARTIST_H

#include "Overlay.h"
#include "Graphics/Styles/ColorMap.h"
#include "Graphics/OpenGL/Texture1D_Color.h"

namespace Graphics::OpenGL {
    typedef std::shared_ptr<Graphics::OpenGL::Texture1D_Color> CMapTexturePtr;
    typedef std::function<Real(Real)> ScalingFunction;

    class ColorBarArtist : public Artist {
        VertexBuffer vertexBuffer;
        Shader shader;
        CMapTexturePtr texture;
        bool textureDirty = true;
        RectI rect;

        ScalingFunction inverseScalingFunction;

        Styles::ColorMap colorMap;

        void updateTexture(int samples=1024);
    public:
        explicit ColorBarArtist(RectI loc);

        void setLocation(RectI loc);
        void setColorMap(const Styles::ColorMap& colorMap);
        auto getTexture() -> CMapTexturePtr;

        void draw(const Graph2D &) override;

        void setInverseScalingFunction(std::function<Real(Real)>);
    };

} // OpenGL

#endif //STUDIOSLAB_COLORBARARTIST_H