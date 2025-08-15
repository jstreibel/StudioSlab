//
// Created by joao on 8/15/25.
//

#ifndef SIMULATIONMANAGER_H
#define SIMULATIONMANAGER_H
#include "Graphics/Backend/Events/SystemWindowEventListener.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"


class FSimulationManager final : public Slab::Graphics::FPlatformWindowEventListener {
public:
    FSimulationManager() = delete;
    explicit FSimulationManager(Slab::TPointer<Slab::Graphics::FImGuiContext>);

    bool NotifyRender(const Slab::Graphics::FPlatformWindow&) override;

private:
    Slab::TPointer<Slab::Graphics::FImGuiContext> ImGuiContext;

};



#endif //SIMULATIONMANAGER_H
