//
// Created by joao on 11/1/24.
//

#include "StudioSlab.h"


class App : public Slab::Application {
protected:
    Slab::Pointer<Slab::Platform> CreatePlatform() override {
        return Slab::DynamicPointerCast<Slab::Graphics::GraphicBackend>(Slab::CreatePlatform("GLFW"));
    }

public:
    App(int argc, char *argv[]) : Slab::Application("Blueprints prototype", argc, argv) { }

protected:
    void OnStart() override {
        Application::OnStart();
    }
};

int main(int argc, char *argv[]) {
    return Slab::Run<App>(argc, argv);
}