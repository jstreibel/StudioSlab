//
// Created by joao on 11/1/24.
//

#include "StudioSlab.h"
#include "Blueprint.h"
#include "BlueprintRenderer.h"
#include "Graphics/SlabGraphics.h"

class App : public Slab::Application {
    Slab::Pointer<Slab::Blueprints::BlueprintRenderer> blueprint_renderer;
    Slab::Pointer<Slab::Blueprints::Blueprint> blueprint;

public:
    App(const int argc, const char *argv[]) : Slab::Application("Blueprints prototype", argc, argv) {
    }

protected:
    Slab::Pointer<Slab::Platform> CreatePlatform() override {
        return Slab::DynamicPointerCast<Slab::Graphics::GraphicBackend>(Slab::CreatePlatform("GLFW"));
    }



    void OnStart() override {
        Application::OnStart();

        using namespace Slab::Blueprints;

        blueprint = Slab::New<Blueprint>();
        auto main_platform_window = Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow();
        blueprint_renderer = Slab::New<BlueprintRenderer>(blueprint, main_platform_window.get());
        addResponder(blueprint_renderer);

        Node* node;
        node = blueprint->SpawnInputActionNode();      ed::SetNodePosition(node->ID, ImVec2(-252, 220));
        node = blueprint->SpawnBranchNode();           ed::SetNodePosition(node->ID, ImVec2(-300, 351));
        node = blueprint->SpawnDoNNode();              ed::SetNodePosition(node->ID, ImVec2(-238, 504));
        node = blueprint->SpawnOutputActionNode();     ed::SetNodePosition(node->ID, ImVec2(71, 80));
        node = blueprint->SpawnSetTimerNode();         ed::SetNodePosition(node->ID, ImVec2(168, 316));

        node = blueprint->SpawnTreeSequenceNode();     ed::SetNodePosition(node->ID, ImVec2(1028, 329));
        node = blueprint->SpawnTreeTaskNode();         ed::SetNodePosition(node->ID, ImVec2(1204, 458));
        node = blueprint->SpawnTreeTask2Node();        ed::SetNodePosition(node->ID, ImVec2(868, 538));

        node = blueprint->SpawnComment();              ed::SetNodePosition(node->ID, ImVec2(112, 576)); ed::SetGroupSize(node->ID, ImVec2(384, 154));
        node = blueprint->SpawnComment();              ed::SetNodePosition(node->ID, ImVec2(800, 224)); ed::SetGroupSize(node->ID, ImVec2(640, 400));

        node = blueprint->SpawnLessNode();             ed::SetNodePosition(node->ID, ImVec2(366, 652));
        node = blueprint->SpawnWeirdNode();            ed::SetNodePosition(node->ID, ImVec2(144, 652));
        node = blueprint->SpawnMessageNode();          ed::SetNodePosition(node->ID, ImVec2(-348, 698));
        node = blueprint->SpawnPrintStringNode();      ed::SetNodePosition(node->ID, ImVec2(-69, 652));

        node = blueprint->SpawnHoudiniTransformNode(); ed::SetNodePosition(node->ID, ImVec2(500, -70));
        node = blueprint->SpawnHoudiniGroupNode();     ed::SetNodePosition(node->ID, ImVec2(500, 42));

        ed::NavigateToContent();

        blueprint->BuildNodes();

        auto nodes = blueprint->GetNodes();

        blueprint->CreateLink(nodes[5].Outputs[0], nodes[6].Outputs[0]);
        blueprint->CreateLink(nodes[5].Outputs[0], nodes[6].Inputs[0]);
        blueprint->CreateLink(nodes[5].Outputs[0], nodes[7].Inputs[0]);

        blueprint->CreateLink(nodes[14].Outputs[0], nodes[15].Inputs[0]);
    }
};

int main(const int argc, const char **argv) {
    return Slab::Run<App>(argc, argv);
}