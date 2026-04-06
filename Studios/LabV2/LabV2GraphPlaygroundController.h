#ifndef STUDIOSLAB_LAB_V2_GRAPH_PLAYGROUND_CONTROLLER_H
#define STUDIOSLAB_LAB_V2_GRAPH_PLAYGROUND_CONTROLLER_H

class FLabV2WindowManager;

class FLabV2GraphPlaygroundController final {
public:
    explicit FLabV2GraphPlaygroundController(FLabV2WindowManager &owner);

    auto DrawPanel() -> void;
    auto MarkDirty() -> void;
    auto SaveStateToFile() -> bool;
    auto LoadStateFromFile() -> bool;

private:
    FLabV2WindowManager &Owner;
};

#endif // STUDIOSLAB_LAB_V2_GRAPH_PLAYGROUND_CONTROLLER_H
