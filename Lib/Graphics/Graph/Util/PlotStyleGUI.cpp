//
// Created by joao on 7/2/24.
//

#include "PlotStyleGUI.h"

#include <utility>
#include "3rdParty/ImGui.h"

namespace Slab::Graphics {

    Str unique_name;

    Str UniqueName(Str name) {
        return name + "##" + unique_name;
    }

    #define Unique(a) UniqueName(a).c_str()

    const char* LinePrimitive_strings[] = {
            "Solid",
            "Dotted",
            "Dashed",
            "Dot-dashed",
            "Vertical lines",
            "Lines",
            "Points",
    };

    const char* get_item(void*, int index) {
        return LinePrimitive_strings[index];
    }

    void DrawPlotStyleGUI(Slab::Graphics::PlotStyle &plot_style, Str name) {
        unique_name = std::move(name);

        auto primitive = (int)plot_style.primitive;

        if(ImGui::Combo(Unique("Primitive"), &primitive, get_item, nullptr, LinePrimitive::__COUNT__))
            plot_style.primitive = (LinePrimitive)primitive;

        ImGui::Checkbox(Unique("Filled"), &plot_style.filled);
        ImGui::DragFloat(Unique("Thickness"), &plot_style.thickness, plot_style.thickness*0.01f, 0.1f, 3e2f, "%.1f");

        ImGui::ColorPicker4(Unique("Line color"), plot_style.lineColor.asFloat4fv());
        if(plot_style.filled)
            ImGui::ColorPicker4(Unique("Fill color"), plot_style.fillColor.asFloat4fv());
    }

}
