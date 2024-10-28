//
// Created by joao on 10/23/24.
//

#include "ImGuiNodesTest2.h"

namespace Tests {


    namespace ed = ax::NodeEditor;

    ImGuiNodesTest2::ImGuiNodesTest2()
    : Slab::Graphics::SlabWindow({"Nodes 2", Slab::Graphics::RectI{1300, 2700, 300, 1100}})
    {
        m_Context = ed::CreateEditor();
    }

    ImGuiNodesTest2::~ImGuiNodesTest2() {
        auto releaseTexture = [this](ImTextureID& id)
        {
            if (id)
            {
                Slab::Graphics::DestroyTexture(id);
                id = nullptr;
            }
        };

        releaseTexture(m_RestoreIcon);
        releaseTexture(m_SaveIcon);
        releaseTexture(m_HeaderBackground);

        if (m_Editor)
        {
            ed::DestroyEditor(m_Editor);
            m_Editor = nullptr;
        }
    }

} // Tests