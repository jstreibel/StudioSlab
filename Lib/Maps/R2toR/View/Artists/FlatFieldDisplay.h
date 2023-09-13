//
// Created by joao on 4/8/23.
//

#ifndef STUDIOSLAB_FLATFIELDDISPLAY_H
#define STUDIOSLAB_FLATFIELDDISPLAY_H

#include <utility>

#include "Core/Graphics/OpenGL/Texture.h"
#include "Maps/R2toR/Model/R2toRFunction.h"
#include "Math/Graph/Graph.h"
#include "Core/Graphics/Styles/ColorMap.h"

namespace R2toR::Graphics {
    class FlatFieldDisplay : public Core::Graphics::Graph2D {
            bool validBuffer = false;

            Styles::ColorMap cMap = Styles::ColorMaps["BrBG"];
            bool logScale = true;
            Real cMap_epsArg = 1;
            Real cMap_min = -1.1;
            Real cMap_max =  1.1;
            bool symmetricMaxMin = true;

        protected:
            R2toR::Function::ConstPtr func    = nullptr;
            OpenGL::Texture*          texture = nullptr;

            Styles::Color computeColor(Real val) const;

            void computeGraphRanges();

        public:
            explicit FlatFieldDisplay(Str title="Full 2D", Real phiMin=-1., Real phiMax=1.)
            : Core::Graphics::Graph2D(-1, 1, -1, 1, std::move(title))
            , cMap_min(phiMin)
            , cMap_max(phiMax)
            , symmetricMaxMin(Common::areEqual(phiMax,-phiMin)) {};
            explicit FlatFieldDisplay(R2toR::Function::ConstPtr function);

            void setup(R2toR::Function::ConstPtr function);
            auto getFunction() const -> R2toR::Function::ConstPtr;

            void setColorMap(Styles::ColorMap colorMap);

            void draw() override;

            void invalidateBuffer();
            void repopulateBuffer();

            bool notifyMouseWheel(int wheel, int direction, int x, int y) override;
            void notifyReshape(int newWinW, int newWinH) override;
    };
}

#endif //STUDIOSLAB_FLATFIELDDISPLAY_H
