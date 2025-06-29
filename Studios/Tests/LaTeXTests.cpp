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
    : Slab::Graphics::WindowRow("LaTeX tests")
    , latexWriter()
    , writer(Core::Resources::GetIndexedFontFileName(10), 19)
    , graph("Graph")
    {
        auto style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[0];
        style.filled = false;
    }

    void LaTeXTests::Draw() {
        constexpr int buffer_size = 64 * 1024;
        static char buffer[buffer_size];

        stats.AddExternalDraw([this]()
        {
            ImGui::Begin("LaTeX");


            if (ImGui::InputText("LaTeX stuff", buffer, buffer_size)) {
                latexWriter.renderMath(StrToWStr(Str(buffer)));
            }

            ImGui::End();
        });

        writer.write(Str(buffer), {200, 1100});
        latexWriter.draw();
    }

    void LaTeXTests::NotifyReshape(int w, int h) {
        writer.reshape(w, h);
        WindowRow::NotifyReshape(w, h);
    }
} // Tests