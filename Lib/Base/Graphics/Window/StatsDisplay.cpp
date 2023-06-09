//
// Created by joao on 8/8/22.
//

#include "StatsDisplay.h"

#include "Base/Graphics/Window/Window.h"
#include "Base/Graphics/OpenGL/GLUTUtils.h"

#include "imgui.h"

#include <GL/gl.h>


StatsDisplay::StatsDisplay() : Window( ) {
    setClear(false);
    setDecorate(false);
}


void StatsDisplay::addVolatileStat(const String &stat, const Styles::Color color)
{
    stats.emplace_back(stat, color);
}

void StatsDisplay::draw() {
    Window::draw();

    auto  displayHeight = ImGui::GetIO().DisplaySize.y;

    float w_ = float(w) - (float)2*winXoffset,
            h_ = float(h) - (float)2*winYoffset;
    float x_ = x+winXoffset;
    float y_ = displayHeight - (h_ + y + winYoffset);

    bool closable=false;

    ImGui::Begin("Stats", &closable,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::SetWindowPos(ImVec2{x_, y_});
    ImGui::SetWindowSize(ImVec2{w_, h_});
    for (auto stat: stats) {
        const auto c = stat.second;
        const auto text = stat.first.c_str();
        const auto color = ImVec4(c.r, c.g, c.b, c.a);

        //ImGui::TextColored(color, text);
        ImGui::Text(text, nullptr);
    }
    ImGui::End();

    if(0) {
        float scaleX = 2.f / w, scaleY = 2.f * 2.f / h;
        glScalef(scaleX, scaleY, 1);
        glTranslatef(-w / 2.f, h / 4.f, 0);


        {
            const int cursorx = 14;
            const float lineHeight = 18;
            float delta = lineHeight;
            auto font = FONT_BITMAP_9;

            auto colorTone = char(0.9 * 255);
            glColor3b(colorTone, colorTone, colorTone);

            for (auto stat: stats) {
                auto color = stat.second;
                glColor3f(color.r, color.g, color.b);
                std::string text = stat.first;
                GLUTUtils::writeBitmap(this, cursorx, -delta, text, font);
                delta += lineHeight;
            }
        }
    }

    stats.clear();
}

