//
// Created by joao on 11/1/24.
//

#include "StudioSlab.h"
#include "Blueprint.h"
#include "BlueprintRenderer.h"
#include "Graphics/SlabGraphics.h"

class App : public Slab::FApplication {
    Slab::TPointer<Slab::Blueprints::FBlueprintRenderer> BlueprintRenderer;
    Slab::TPointer<Slab::Blueprints::FBlueprint> Blueprint;

public:
    App(const int argc, const char *argv[])
    : Slab::FApplication("Blueprints prototype", argc, argv) {
    }

protected:
    Slab::TPointer<Slab::Platform> CreatePlatform() override {
        return Slab::DynamicPointerCast<Slab::Graphics::GraphicBackend>(Slab::CreatePlatform("GLFW"));
    }



    void OnStart() override {
        FApplication::OnStart();

        using namespace Slab::Blueprints;

        Blueprint = Slab::New<FBlueprint>();
        auto main_platform_window = Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow();

        auto GUIContext = Slab::DynamicPointerCast<Slab::Graphics::FImGuiContext>(
            Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow()->SetupGUIContext());

        BlueprintRenderer = Slab::New<FBlueprintRenderer>(Blueprint, GUIContext);
        // main_platform_window->AddEventListener(blueprint_renderer);
        AddResponder(BlueprintRenderer);

        FBlueprintNode* Node;
        Node = Blueprint->SpawnInputActionNode();      ed::SetNodePosition(Node->ID, ImVec2(-252, 220));
        Node = Blueprint->SpawnBranchNode();           ed::SetNodePosition(Node->ID, ImVec2(-300, 351));
        Node = Blueprint->SpawnDoNNode();              ed::SetNodePosition(Node->ID, ImVec2(-238, 504));
        Node = Blueprint->SpawnOutputActionNode();     ed::SetNodePosition(Node->ID, ImVec2(71, 80));
        Node = Blueprint->SpawnSetTimerNode();         ed::SetNodePosition(Node->ID, ImVec2(168, 316));

        Node = Blueprint->SpawnTreeSequenceNode();     ed::SetNodePosition(Node->ID, ImVec2(1028, 329));
        Node = Blueprint->SpawnTreeTaskNode();         ed::SetNodePosition(Node->ID, ImVec2(1204, 458));
        Node = Blueprint->SpawnTreeTask2Node();        ed::SetNodePosition(Node->ID, ImVec2(868, 538));

        Node = Blueprint->SpawnComment();              ed::SetNodePosition(Node->ID, ImVec2(112, 576)); ed::SetGroupSize(Node->ID, ImVec2(384, 154));
        Node = Blueprint->SpawnComment();              ed::SetNodePosition(Node->ID, ImVec2(800, 224)); ed::SetGroupSize(Node->ID, ImVec2(640, 400));

        Node = Blueprint->SpawnLessNode();             ed::SetNodePosition(Node->ID, ImVec2(366, 652));
        Node = Blueprint->SpawnWeirdNode();            ed::SetNodePosition(Node->ID, ImVec2(144, 652));
        Node = Blueprint->SpawnMessageNode();          ed::SetNodePosition(Node->ID, ImVec2(-348, 698));
        Node = Blueprint->SpawnPrintStringNode();      ed::SetNodePosition(Node->ID, ImVec2(-69, 652));

        Node = Blueprint->SpawnHoudiniTransformNode(); ed::SetNodePosition(Node->ID, ImVec2(500, -70));
        Node = Blueprint->SpawnHoudiniGroupNode();     ed::SetNodePosition(Node->ID, ImVec2(500, 42));

        ed::NavigateToContent();

        Blueprint->BuildNodes();

        auto nodes = Blueprint->GetNodes();

        Blueprint->CreateLink(nodes[5].Outputs[0], nodes[6].Outputs[0]);
        Blueprint->CreateLink(nodes[5].Outputs[0], nodes[6].Inputs[0]);
        Blueprint->CreateLink(nodes[5].Outputs[0], nodes[7].Inputs[0]);

        Blueprint->CreateLink(nodes[14].Outputs[0], nodes[15].Inputs[0]);
    }
};

int main(const int argc, const char **argv) {
    return Slab::Run<App>(argc, argv);
}