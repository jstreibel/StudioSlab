//
// Created by joao on 18/08/23.
//

#ifndef STUDIOSLAB_HISTORYDISPLAY_H
#define STUDIOSLAB_HISTORYDISPLAY_H

#include "Maps/R2toR/View/Artists/FlatFieldDisplay.h"


namespace RtoR::Graphics {

    class HistoryDisplay : public R2toR::Graphics::FlatFieldDisplay {
    public:
        void set_t(Real t);
    };

}


#endif //STUDIOSLAB_HISTORYDISPLAY_H
