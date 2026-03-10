#include "LabV2GraphPlaygroundController.h"

#include "LabV2WindowManager.h"

FLabV2GraphPlaygroundController::FLabV2GraphPlaygroundController(FLabV2WindowManager &owner)
: Owner(owner) {
}

auto FLabV2GraphPlaygroundController::DrawPanel() -> void {
    Owner.DrawGraphPlaygroundPanelImpl();
}

auto FLabV2GraphPlaygroundController::MarkDirty() -> void {
    Owner.MarkGraphPlaygroundDirtyImpl();
}

auto FLabV2GraphPlaygroundController::SaveStateToFile() -> bool {
    return Owner.SaveGraphPlaygroundStateToFileImpl();
}

auto FLabV2GraphPlaygroundController::LoadStateFromFile() -> bool {
    return Owner.LoadGraphPlaygroundStateFromFileImpl();
}
