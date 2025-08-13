//
// Created by joao on 31/08/23.
//

#ifndef STUDIOSLAB_LABELSARTIST_H
#define STUDIOSLAB_LABELSARTIST_H

#include "Utils/Types.h"
#include "Graphics/Plot2D/PlotStyle.h"
#include "Graphics/Plot2D/Artists/Artist.h"

namespace Slab::Graphics {

    class FLabelsArtist : public FArtist {
        CountType currItem=0;
        CountType cols = 1;
        int max_cols = 4;

        Vector<Pair<Str, TPointer<PlotStyle>>> labelsRequired;

        float dx = .080f,
              dy = .060f;
        float x_offset = .1f,
              y_offset = .975f;
        float xGap = 0.015f,
              yGap = .025f;

        void setTotalItems(CountType tot);

        [[nodiscard]] CountType row() const;
        [[nodiscard]] CountType col() const;

        FLabelsArtist& operator++();

        void draw_label(PlotStyle &style, const Str& label, const RectI &viewport);

    public:

        bool Draw(const FPlot2DWindow &window) override;
        void add(const Str& label, const TPointer<PlotStyle>& style);

        bool HasGUI() override;

        void DrawGUI() override;
    };

}


#endif //STUDIOSLAB_LABELSARTIST_H
