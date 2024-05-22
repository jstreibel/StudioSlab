//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_GRAPH_H
#define STUDIOSLAB_GRAPH_H

#include <memory>
#include <list>

#include "Utils/Utils.h"

#include "LabelingHelper.h"

#include "PlotStyle.h"

#include "Graphics/Utils.h"
#include "Graphics/Window/Window.h"
#include "Graphics/OpenGL/Utils.h"
#include "Graphics/OpenGL/Writer.h"

#include "Artists/AxisArtist.h"
#include "Artists/XHairArtist.h"

namespace Graphics {

    class Graph2D : public Window {
    protected:
        typedef Int zOrder_t;

    private:
        static std::map<Str, Graph2D*> graphMap;

        typedef std::multimap<zOrder_t, Artist_ptr> ContentMap;
        ContentMap content;

        bool savePopupOn = false;
        bool autoReviewGraphRanges=false;

        RectR region;

        Real animationTimeSeconds = 0.2;

        Math::PointSet XHair;

    protected:
        Str title;

        Math::Graphics::LabelingHelper labelingHelper;

        virtual auto countDisplayItems() const -> Count;
        void nameLabelDraw(const PlotStyle &style, const Str& label);

        void artistsDraw();
        virtual void drawGUI();

        friend class Artist;;

        AxisArtist axisArtist;
        XHairArtist artistXHair;

    public:
        explicit Graph2D(Real xMin=-1, Real xMax=1, Real yMin=-1, Real yMax=1,
                Str title = "no_title");

        explicit Graph2D(Str title, bool autoReviewGraphLimits=true);

        void draw() override;

        void addArtist(const Artist_ptr& pArtist, zOrder_t zOrder=0);
        bool removeArtist(const Artist_ptr& pArtist);

        AxisArtist &getAxisArtist();

        auto getLastXHairPosition() const -> Point2D;
        virtual Str getXHairLabel(const Point2D &coords) const;

        void setupOrtho() const;
        void reviewGraphRanges();

        auto getRegion() const -> const RectR&;
        auto setLimits(RectR limits) -> void;
        void set_xMin(Real);
        void set_xMax(Real);
        void set_yMin(Real);
        void set_yMax(Real);
        Real get_xMin() const;
        Real get_xMax() const;
        Real get_yMin() const;
        Real get_yMax() const;


        void setAnimationTime(Real value);
        Real getAnimationTime() const;

        bool notifyMouseButton(Core::MouseButton button, Core::KeyState state, Core::ModKeys keys) override;
        bool notifyMouseWheel(double dx, double dy) override;
        bool notifyMouseMotion(int x, int y) override;

        void notifyReshape(int newWinW, int newWinH) override;

    };

    DefinePointer(Graph2D)

}




#endif //STUDIOSLAB_GRAPH_H
