//
// Created by joao on 9/26/24.
//

#include "LaTeXTests.h"

#include "Core/Tools/Log.h"
#include "Core/Tools/Resources.h"

#include "Graphics/Plot2D/PlotThemeManager.h"

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
        if(ImGui::InputText("LaTeX stuff", buffer, buffer_size)){
            latexWriter.renderMath(StrToWStr(Str(buffer)));
        }

        ImGui::End();

        writer.write(Str(buffer), {200, 1100});
        latexWriter.draw();
    }

    void LaTeXTests::notifyReshape(int w, int h) {
        writer.reshape(w, h);
        WindowRow::notifyReshape(w, h);
    }
} // Tests