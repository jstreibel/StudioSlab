#include <catch2/catch_all.hpp>

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/Window/WindowContainer/WindowRow.h"

namespace {

using namespace Slab;
using namespace Slab::Graphics;

class FInteractiveLeaf final : public FSlabWindow {
  public:
    FInteractiveLeaf() : FSlabWindow(FSlabWindowConfig{"Interactive leaf"}) {}

    bool NotifyMouseButton(EMouseButton button, EKeyState state, EModKeys keys) override {
        ++ButtonEventCount;
        return FSlabWindow::NotifyMouseButton(button, state, keys);
    }

    bool NotifyMouseMotion(int x, int y, int dx, int dy) override {
        FSlabWindow::NotifyMouseMotion(x, y, dx, dy);

        if (GetMouseState()->IsLeftPressed() || GetMouseState()->IsRightPressed()) {
            ++DragMotionCount;
            return true;
        }

        return false;
    }

    bool NotifyMouseWheel(double dx, double dy) override {
        ++WheelEventCount;
        return FSlabWindow::NotifyMouseWheel(dx, dy);
    }

    int ButtonEventCount = 0;
    int DragMotionCount = 0;
    int WheelEventCount = 0;
};

} // namespace

TEST_CASE("Nested window panels propagate plot-style mouse gestures", "[Window][Input]") {
    auto leaf = Slab::New<FInteractiveLeaf>();
    auto panel = Slab::New<Slab::Graphics::FWindowPanel>(Slab::Graphics::FSlabWindowConfig{"Panel"});
    panel->AddWindow(leaf);

    Slab::Graphics::FWindowRow row("Row");
    REQUIRE(row.AddWindow(panel));
    row.Set_x(0);
    row.Set_y(0);
    row.NotifyReshape(1000, 800);

    const auto x = leaf->Get_x() + leaf->GetWidth() / 2;
    const auto y = leaf->Get_y() + leaf->GetHeight() / 2;

    CHECK_FALSE(row.NotifyMouseMotion(x, y, 0, 0));
    CHECK(panel->GetMouseState()->x == x);
    CHECK(panel->GetMouseState()->y == y);

    CHECK_FALSE(
        row.NotifyMouseButton(Slab::Graphics::EMouseButton::MouseButton_LEFT, Slab::Graphics::EKeyState::Press, {}));
    REQUIRE(leaf->GetMouseState()->IsLeftPressed());
    CHECK(panel->GetMouseState()->IsLeftPressed());

    CHECK(row.NotifyMouseMotion(x + 20, y + 10, 20, 10));
    CHECK(leaf->DragMotionCount == 1);

    CHECK_FALSE(
        row.NotifyMouseButton(Slab::Graphics::EMouseButton::MouseButton_LEFT, Slab::Graphics::EKeyState::Release, {}));
    CHECK_FALSE(leaf->GetMouseState()->IsLeftPressed());
    CHECK_FALSE(panel->GetMouseState()->IsLeftPressed());

    CHECK_FALSE(
        row.NotifyMouseButton(Slab::Graphics::EMouseButton::MouseButton_RIGHT, Slab::Graphics::EKeyState::Press, {}));
    REQUIRE(leaf->GetMouseState()->IsRightPressed());

    CHECK(row.NotifyMouseMotion(x + 10, y + 20, -10, 10));
    CHECK(leaf->DragMotionCount == 2);

    CHECK_FALSE(
        row.NotifyMouseButton(Slab::Graphics::EMouseButton::MouseButton_RIGHT, Slab::Graphics::EKeyState::Release, {}));
    CHECK_FALSE(leaf->GetMouseState()->IsRightPressed());

    CHECK_FALSE(row.NotifyMouseWheel(0.0, 1.0));
    CHECK(leaf->WheelEventCount == 1);
    CHECK(panel->GetMouseState()->wheel_dy == 1);
    CHECK(leaf->ButtonEventCount == 4);
}
