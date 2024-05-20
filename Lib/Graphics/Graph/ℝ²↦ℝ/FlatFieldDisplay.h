//
// Created by joao on 4/8/23.
//

#ifndef STUDIOSLAB_FLATFIELDDISPLAY_H
#define STUDIOSLAB_FLATFIELDDISPLAY_H

#include <utility>

#include "FlatField2DArtist.h"

#include "Graphics/Styles/ColorMap.h"

#include "Graphics/Graph/Graph.h"

#include "Math/Function/R2toR/Model/R2toRFunction.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunction.h"

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

            ColorMap currColorMap;
            typedef std::shared_ptr<Graphics::FlatField2DArtist> FlatField2DArtistPtr;

        protected:
            std::vector<FlatField2DArtistPtr> ff2dArtists;

            Unit funcUnit;
            typedef std::multimap<zOrder_t, R2toR::Function::ConstPtr> FuncsMap;
            FuncsMap funcsMap;
            bool ContainsFunc(const R2toR::Function::ConstPtr&);

            void computeGraphRanges(const R2toR::Domain &domain);

            Str getXHairLabel(const Graphics::Point2D &coords) const override;

    public:
            explicit FlatFieldDisplay(Str title="Full 2D");

            std::shared_ptr<FlatField2DArtist> addFunction(R2toR::Function::ConstPtr function, const Str& name, zOrder_t zOrder=0);

            bool removeFunction(R2toR::Function::ConstPtr function);

            auto getFunctionsMap() const -> FuncsMap;


            void setColorMap(const ColorMap& colorMap);

            bool notifyMouseWheel(double dx, double dy) override;

            void notifyReshape(int newWinW, int newWinH) override;

            void set_xPeriodicOn();

    };
}

#endif //STUDIOSLAB_FLATFIELDDISPLAY_H
