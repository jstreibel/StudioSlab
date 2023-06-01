//
// Created by joao on 8/8/22.
//

#ifndef STUDIOSLAB_STATSDISPLAY_H
#define STUDIOSLAB_STATSDISPLAY_H


#include "Base/Graphics/PlottingUtils.h"
#include "Base/Graphics/WindowManagement/Window.h"

#include <Common/Typedefs.h>


class StatsDisplay : public Window {
    std::vector<std::pair<String, Color>> stats;

public:
    explicit StatsDisplay( );

    void addVolatileStat(const String& stat, const Color color = {1, 1, 1});

    void draw() override;


};


#endif //STUDIOSLAB_STATSDISPLAY_H
