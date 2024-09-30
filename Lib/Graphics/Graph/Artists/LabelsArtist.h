//
// Created by joao on 31/08/23.
//

#ifndef STUDIOSLAB_LABELSARTIST_H
#define STUDIOSLAB_LABELSARTIST_H

#include "Utils/Types.h"
#include "Graphics/Graph/PlotStyle.h"
#include "Graphics/Graph/Artists/Artist.h"

namespace Slab::Graphics {

    class LabelsArtist : public Artist {
        Count currItem=0;
        Count cols = 1;
        int max_cols = 4;

        Vector<Pair<Str, Pointer<PlotStyle>>> labelsRequired;

        float dx = .080f,
              dy = .060f;
        float x_offset = .1f,
              y_offset = .975f;
        float xGap = 0.015f,
              yGap = .025f;

        void setTotalItems(Count tot);

        Count row() const;
        Count col() const;

        LabelsArtist& operator++();

        void draw_label(PlotStyle &style, const Str& label, const RectI &viewport);

    public:

        bool draw(const PlottingWindow &window) override;
        void add(const Str& label, const Pointer<PlotStyle>& style);

        bool hasGUI() override;

        void drawGUI() override;
    };

}


#endif //STUDIOSLAB_LABELSARTIST_H
