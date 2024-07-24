//
// Created by joao on 7/24/24.
//

#include "VShapeExpansionTest.h"
#include "Utils/Pointer.h"

namespace Tests {
    VShapeExpansionTest::VShapeExpansionTest() : Slab::Graphics::WindowRow(HasMainMenu) {
        addWindow(Slab::Naked(stats));
    }

    void VShapeExpansionTest::draw() {
        stats.begin();
        ImGui::Text("O HAI", nullptr);
        stats.end();

        WindowRow::draw();
    }
} // Tests