//
// Created by joao on 8/8/22.
//

#ifndef STUDIOSLAB_GUIWINDOW_H
#define STUDIOSLAB_GUIWINDOW_H


#include "Graphics/Utils.h"
#include "Graphics/Window/Window.h"

#include "Utils/Types.h"


namespace Slab::Graphics {

    class GUIWindow : public Window {
        std::vector<std::pair<Str, Color>> stats;

    public:
        explicit GUIWindow();

        void addVolatileStat(const Str &stat, Color color = {0, 0, 0});

        void draw() override;

        void begin() const;

        void end() const;

    };


}

#endif //STUDIOSLAB_GUIWINDOW_H
