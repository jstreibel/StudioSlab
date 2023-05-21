//
// Created by joao on 28/05/2021.
//

#include "Graph_deprCollections.h"



Graph_depr &Base::getPhiGraph(Base::SectionPair &sectionPair) { return sectionPair.first.first; }
Graph_depr &Base::getDtPhiGraph(Base::SectionPair &sectionPair) { return sectionPair.first.second; }
RtoR2::StraightLine &Base::getLine(Base::SectionPair &sectionPair) { return sectionPair.second.first; }
Color &Base::getColor(Base::SectionPair &sectionPair) { return sectionPair.second.second; }

const Real alpha = 0.5f;
std::vector<Color> Base::colors = {Color(.2f, .2f, .4f, alpha), Color(.4f, .4f, 0.2f, alpha),
                                    Color(.4f, .2f, .2f, alpha), Color(.2f, .4f, 0.2f, alpha)};
