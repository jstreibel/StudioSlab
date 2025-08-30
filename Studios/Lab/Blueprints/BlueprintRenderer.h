//
// Created by joao on 11/2/24.
//

#ifndef STUDIOSLAB_BLUEPRINTRENDERER_H
#define STUDIOSLAB_BLUEPRINTRENDERER_H

#include <map>

#include "BlueprintTypes.h"
#include "Blueprint.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"

namespace Lab::Blueprints {

    using namespace Slab;

    class FBlueprintRenderer : public Graphics::FPlatformWindowEventListener {
    public:
        explicit FBlueprintRenderer(TPointer<FBlueprint> blueprint, TPointer<Graphics::FImGuiContext>);

        static ImColor GetIconColor(PinType type);;

        void DrawPinIcon(const Pin& pin, bool connected, int alpha) const;;

        static void ShowStyleEditor(bool* show = nullptr);

        void ShowLeftPane(float paneWidth);

        bool NotifyRender(const Graphics::FPlatformWindow&) override;

    private:
        struct FRendererState
        {
            Editor::NodeId ContextNodeId = 0;
            Editor::LinkId ContextLinkId = 0;
            Editor::PinId ContextPinId = 0;
            bool bCreateNewNode = false;
            Pin *NewNodeLinkPin = nullptr;
            Pin *NewLinkPin = nullptr;

            float LeftPaneWidth = 400.0f;
            float RightPaneWidth = 800.0f;
        } MyState;

        Editor::EditorContext* m_Editor = nullptr;

        TPointer<Graphics::FImGuiContext> m_Context;
        const int            m_PinIconSize = 24;
        ImTextureID          m_HeaderBackground = nullptr;
        ImTextureID          m_SaveIcon = nullptr;
        ImTextureID          m_RestoreIcon = nullptr;
        const float          m_TouchTime = 1.0f;
        std::map<Editor::NodeId, float, NodeIdLess> m_NodeTouchTime;
        bool                 m_ShowOrdinals = false;

        EditorUtil::BlueprintNodeBuilder Builder;
        TPointer<FBlueprint> Blueprint;

        void TouchNode(Editor::NodeId id);

        float GetTouchProgress(Editor::NodeId id);

        void UpdateTouch();

        void HandleNodeCreation();
        void DoDrawing();

        static void ShowOrdinals();

        void HandleBlueprintOrSimpleNodeType(FBlueprintNode&);
        void HandleTreeNodeType(FBlueprintNode&) const;
        void HandleHoudiniNodeType(FBlueprintNode&) const;
        static void HandleCommentNodeType(FBlueprintNode&);

        void ShowContextMenus();
    };

} // Slab::Blueprints

#endif //STUDIOSLAB_BLUEPRINTRENDERER_H
