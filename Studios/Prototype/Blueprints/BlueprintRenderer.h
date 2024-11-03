//
// Created by joao on 11/2/24.
//

#ifndef STUDIOSLAB_BLUEPRINTRENDERER_H
#define STUDIOSLAB_BLUEPRINTRENDERER_H

#include <map>

#include "3rdParty/ImGui.h"

#include "BlueprintTypes.h"
#include "Graphics/OpenGL/Images.h"
#include "Blueprint.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"

namespace Slab::Blueprints {

    class BlueprintRenderer {
        ed::EditorContext* m_Editor = nullptr;

        Pointer<Graphics::SlabImGuiContext> m_Context;
        const int            m_PinIconSize = 24;
        ImTextureID          m_HeaderBackground = nullptr;
        ImTextureID          m_SaveIcon = nullptr;
        ImTextureID          m_RestoreIcon = nullptr;
        const float          m_TouchTime = 1.0f;
        std::map<ed::NodeId, float, NodeIdLess> m_NodeTouchTime;
        bool                 m_ShowOrdinals = false;

        Pointer<Blueprint> blueprint;

        void TouchNode(ed::NodeId id);

        float GetTouchProgress(ed::NodeId id);

        void UpdateTouch();

    public:
        explicit BlueprintRenderer(Pointer<Blueprint> blueprint);

        ImColor GetIconColor(PinType type);;

        void DrawPinIcon(const Pin& pin, bool connected, int alpha);;

        void ShowStyleEditor(bool* show = nullptr);

        void ShowLeftPane(float paneWidth);

        void Draw();

    };

} // Slab::Blueprints

#endif //STUDIOSLAB_BLUEPRINTRENDERER_H
