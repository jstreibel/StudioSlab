//
// Created by joao on 4/8/23.
//

#ifndef STUDIOSLAB_FLATFIELDDISPLAY_H
#define STUDIOSLAB_FLATFIELDDISPLAY_H

#include <utility>

#include "FlatField2DArtist.h"

#include "Graphics/Styles/ColorMap.h"

#include "Graphics/Graph/Graph.h"

#include "Maps/R2toR/Model/R2toRFunction.h"
#include "Maps/R2toR/Model/R2toRDiscreteFunction.h"

#include "Graphics/OpenGL/Texture2D_Color.h"
#include "Graphics/OpenGL/Texture2D_Real.h"
#include "Graphics/OpenGL/Texture1D_Color.h"
#include "Graphics/OpenGL/Artists/ColorBarArtist.h"

#include "Math/Constants.h"


namespace Graphics {
    class FlatFieldDisplay : public Graphics::Graph2D {
            enum Scale {
                Linear,
                Log,
                Logit,
                LogEpsilon
            };

            Graphics::OpenGL::ColorBarArtist colorBar;
            Graphics::FlatField2DArtist flatField2DArtist;

        protected:
            Unit funcUnit;
            R2toR::Function::ConstPtr func    = nullptr;

            void computeGraphRanges(const R2toR::Domain &domain);

            Str getXHairLabel(const Graphics::Point2D &coords) const override;

        public:
            explicit FlatFieldDisplay(Str title="Full 2D", Real phiMin=-1., Real phiMax=1.);

            void setFunction(R2toR::Function::ConstPtr function, const Unit& unit=Constants::One);

            auto getFunction() const -> R2toR::Function::ConstPtr;

            void setColorMap(const Styles::ColorMap& colorMap);

            bool notifyMouseWheel(double dx, double dy) override;

            void notifyReshape(int newWinW, int newWinH) override;

            void set_xPeriodicOn();

    };
}

#endif //STUDIOSLAB_FLATFIELDDISPLAY_H
