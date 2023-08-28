//
// Created by joao on 8/8/22.
//

#ifndef STUDIOSLAB_STATSDISPLAY_H
#define STUDIOSLAB_STATSDISPLAY_H


#include "Core/Graphics/Utils.h"
#include "Core/Graphics/Window/Window.h"

#include "Utils/Types.h"


class StatsDisplay : public Window {
    std::vector<std::pair<Str, Styles::Color>> stats;

public:
    explicit StatsDisplay( );

    void addVolatileStat(const Str& stat, const Styles::Color color = {1, 1, 1});

    void draw() override;

    void begin() const;
    void end() const;

};


#endif //STUDIOSLAB_STATSDISPLAY_H
