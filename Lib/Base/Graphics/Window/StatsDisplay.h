//
// Created by joao on 8/8/22.
//

#ifndef STUDIOSLAB_STATSDISPLAY_H
#define STUDIOSLAB_STATSDISPLAY_H


#include "Base/Graphics/Utils.h"
#include "Base/Graphics/Window/Window.h"

#include "Common/Types.h"


class StatsDisplay : public Window {
    std::vector<std::pair<Str, Styles::Color>> stats;

public:
    explicit StatsDisplay( );

    void addVolatileStat(const Str& stat, const Styles::Color color = {1, 1, 1});

    void draw() override;


};


#endif //STUDIOSLAB_STATSDISPLAY_H
