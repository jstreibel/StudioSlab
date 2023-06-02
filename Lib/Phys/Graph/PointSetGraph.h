//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_POINTSETGRAPH_H
#define STUDIOSLAB_POINTSETGRAPH_H


#include "Graph.h"
#include "Phys/Space/Impl/PointSet.h"

namespace Phys {
    namespace Graphing {

        class PointSetGraph : public Base::Graphics::Graph2D {

            typedef std::tuple<Spaces::PointSet::Ptr, Styles::PlotStyle, String> PointSetTriple;
            static auto GetPointSet ( PointSetTriple triple ) { return std::get<0>(triple); };
            static auto GetStyle    ( PointSetTriple triple ) { return std::get<1>(triple); };
            static auto GetName     ( PointSetTriple triple ) { return std::get<2>(triple); };


            typedef std::vector<PointSetTriple> PointSets;

            PointSets mPointSets;

            void _reviewGraphRanges();

            void _renderPointSet(const Spaces::PointSet &pSet, Styles::PlotStyle style) const noexcept;
        public:
            PointSetGraph(const String &title);

            void draw() override;

            void addPointSet(Spaces::PointSet::Ptr pointSet, Styles::PlotStyle style, String setName);

        };

    }
}




#endif //STUDIOSLAB_POINTSETGRAPH_H
