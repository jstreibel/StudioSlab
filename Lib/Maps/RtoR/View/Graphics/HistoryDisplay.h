//
// Created by joao on 18/08/23.
//

#ifndef STUDIOSLAB_HISTORYDISPLAY_H
#define STUDIOSLAB_HISTORYDISPLAY_H

#include "Maps/R2toR/View/Artists/FlatFieldDisplay.h"


namespace RtoR::Graphics {

    class HistoryDisplay : public R2toR::Graphics::FlatFieldDisplay {
        Count nextRow = 0;
        Real lastUpdatedTime = -1;

    public:
        explicit HistoryDisplay(Str name="Full history", Real phiMin=-1., Real phiMax=1.);

        void set_t(Real t);
    };

}


#endif //STUDIOSLAB_HISTORYDISPLAY_H
