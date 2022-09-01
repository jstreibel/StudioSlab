//
// Created by joao on 8/8/22.
//

#ifndef STUDIOSLAB_STATSDISPLAY_H
#define STUDIOSLAB_STATSDISPLAY_H


#include "Artist.h"
#include "Studios/Graphics/PlottingUtils.h"

#include <Common/Typedefs.h>


class StatsDisplay : public Artist {
    std::vector<std::pair<String, Color>> stats;

public:
    void addVolatileStat(const String& stat, const Color color = {1, 1, 1});
    void draw(const Window *window) override;

};


#endif //STUDIOSLAB_STATSDISPLAY_H
