//
// Created by joao on 9/26/24.
//

#include "LaTeXTests.h"

#include "Core/Tools/Resources.h"

#include "Graphics/Graph/PlottingWindow.h"
#include "Graphics/Graph/PlotThemeManager.h"
#include "Graphics/OpenGL/Writer.h"

namespace Tests {
    using namespace Slab;

    LaTeXTests::LaTeXTests()
    : Slab::Graphics::WindowRow(Flags::HasMainMenu)
    , latexWriter()
    , writer(Core::Resources::fontFileName(10), 19)
    {
        auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0];
        style.filled = false;
    }

    void LaTeXTests::draw() {
        ImGui::Begin("LaTeX");

        constexpr int buffer_size = 64*1024;
        static char buffer[buffer_size];
        ImGui::InputText("LaTeX stuff", buffer, buffer_size);

        ImGui::End();

        auto str_buffer = Str(buffer);
        latexWriter.renderMath(StrToWStr(str_buffer));

        writer.write(str_buffer, {200, 1100});
    }

    void LaTeXTests::notifyReshape(int w, int h) {
        writer.reshape(w, h);
        WindowRow::notifyReshape(w, h);
    }
} // Tests