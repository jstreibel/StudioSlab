//
// Created by joao on 8/8/22.
//

#include "StatsDisplay.h"

#include "Base/Graphics/WindowManagement/Window.h"
#include "Base/Graphics/OpenGL/GLUTUtils.h"

#include "imgui.h"

#include <GL/gl.h>



void StatsDisplay::addVolatileStat(const String &stat, const Color color)
{
    stats.emplace_back(stat, color);
}

void StatsDisplay::draw(const Window *window) {
    auto  displayHeight = ImGui::GetIO().DisplaySize.y;

    float w = float(window->w) - (float)2*window->winXoffset,
          h = float(window->h) - (float)2*window->winYoffset;
    float x = window->x+window->winXoffset;
    float y = displayHeight - (h + window->y + window->winYoffset);

    bool closable=false;

    ImGui::Begin("Stats", &closable,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::SetWindowPos(ImVec2{x, y});
    ImGui::SetWindowSize(ImVec2{w, h});
    for (auto stat: stats) {
        const auto c = stat.second;
        const auto text = stat.first.c_str();
        const auto color = ImVec4(c.r, c.g, c.b, c.a);

        //ImGui::TextColored(color, text);
        ImGui::Text(text);
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
                GLUTUtils::writeBitmap(window, cursorx, -delta, text, font);
                delta += lineHeight;
            }
        }
    }

    stats.clear();
}
