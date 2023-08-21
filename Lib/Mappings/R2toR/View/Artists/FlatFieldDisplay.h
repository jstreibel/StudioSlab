//
// Created by joao on 4/8/23.
//

#ifndef STUDIOSLAB_FLATFIELDDISPLAY_H
#define STUDIOSLAB_FLATFIELDDISPLAY_H

#include "Base/Graphics/Window/Window.h"
#include "Base/Graphics/OpenGL/Texture.h"

#include "Phys/Graph/Graph.h"
#include "Mappings/R2toR/Model/R2toRFunction.h"
#include "Base/Graphics/Styles/ColorMap.h"

namespace R2toR::Graphics {
    class FlatFieldDisplay : public Base::Graphics::Graph2D {
            bool validBuffer = false;

            Styles::ColorMap cMap = Styles::ColorMaps["BrBG"];
            bool logScale = true;
            Real cMap_epsArg = 0.1;
            Real cMap_min = -.2;
            Real cMap_max =  .2;


        protected:
            R2toR::Function::ConstPtr func    = nullptr;
            OpenGL::Texture*          texture = nullptr;

            Styles::Color computeColor(Real val) const;

            void computeGraphRanges();
        public:
            FlatFieldDisplay(Str title="Full 2D") : Base::Graphics::Graph2D(-1,1,-1,1,title) {};
            explicit FlatFieldDisplay(R2toR::Function::ConstPtr function);

            void setup(R2toR::Function::ConstPtr function);

            virtual void draw() override;

            void invalidateBuffer();

            void repopulateBuffer();

            bool notifyMouseWheel(int wheel, int direction, int x, int y) override;

            void notifyReshape(int newWinW, int newWinH) override;

        bool notifyScreenReshape(int newScreenWidth, int newScreenHeight) override;
    };
}

#endif //STUDIOSLAB_FLATFIELDDISPLAY_H
