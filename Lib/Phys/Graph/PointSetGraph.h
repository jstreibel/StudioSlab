//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_POINTSETGRAPH_H
#define STUDIOSLAB_POINTSETGRAPH_H


#include "Graph.h"
#include "Phys/Space/Impl/PointSet.h"

namespace Fields {
    namespace Graphing {

        class PointSetGraph : public Base::Graphics::Graph2D {

            struct PointSetMetadata {
                Spaces::PointSet::Ptr data;
                Styles::PlotStyle plotStyle;
                Str name;
                bool affectsGraphRanges=true;
            };

            typedef std::vector<PointSetMetadata> PointSets;

            PointSets mPointSets;

            bool autoReviewGraphRanges;

            void _renderPointSet(const Spaces::PointSet &pSet, Styles::PlotStyle style) const noexcept;
        public:
            explicit PointSetGraph(const Str &title, bool autoReviewGraphRanges=true);

            void reviewGraphRanges();
            void draw() override;

            void addPointSet(Spaces::PointSet::Ptr pointSet,
                             Styles::PlotStyle style,
                             Str setName,
                             bool affectsGraphRanges=true);

        };

    }
}




#endif //STUDIOSLAB_POINTSETGRAPH_H
