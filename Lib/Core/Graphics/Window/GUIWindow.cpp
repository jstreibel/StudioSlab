//
// Created by joao on 8/8/22.
//

#include "GUIWindow.h"

#include "imgui.h"
#include "Core/Graphics/Styles/WindowStyles.h"


GUIWindow::GUIWindow() : Window( ) {
    setClear(false);
    setDecorate(false);
}


void GUIWindow::addVolatileStat(const Str &stat, const Styles::Color color)
{
    stats.emplace_back(stat, color);
}

void GUIWindow::draw() {
    Window::draw();

    auto  displayHeight = ImGui::GetIO().DisplaySize.y;

    const float w_ = float(getw()) - (float)2 * Core::Graphics::hPadding,
                h_ = float(geth()) - (float)2 * Core::Graphics::vPadding;
    const float x_ = getx() + Core::Graphics::hPadding,
                y_ = displayHeight - (h_ + gety() + Core::Graphics::vPadding);

    const float hSpacing = 20.0f;

    begin();

    ImGui::SetWindowPos( ImVec2{x_, y_});
    ImGui::SetWindowSize(ImVec2{w_, h_});

    if(ImGui::CollapsingHeader("Stats", ImGuiTreeNodeFlags_DefaultOpen)) {

        auto i = 0;
        for (auto stat: stats) {
            const auto c = stat.second;
            const auto text = stat.first;
            const auto color = ImVec4(c.r, c.g, c.b, c.a);

            if (text == "<\\br>") {
                ImGui::Separator();
                i = 0;
                continue;
            }

            if (i++ % 2 && w_ > 500) ImGui::SameLine(getw() / 2. - hSpacing);

            ImGui::TextColored(color, text.c_str(), nullptr);
        }
    }

    end();

    if(0) {
        float scaleX = 2.f / getw(), scaleY = 2.f * 2.f / geth();
        glScalef(scaleX, scaleY, 1);
        glTranslatef(-getw() / 2.f, geth() / 4.f, 0);


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

void GUIWindow::begin() const {
    bool closable=false;

    ImGui::Begin("Stats", &closable,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
}

void GUIWindow::end() const {
    ImGui::End();
}

