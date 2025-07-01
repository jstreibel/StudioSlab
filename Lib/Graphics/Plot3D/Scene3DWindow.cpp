//
// Created by joao on 4/10/23.
//

#include <glm/ext/matrix_transform.hpp>
#include "Scene3DWindow.h"
#include "Graphics/Plot3D/Actors/R2toRFunctionActor.h"
#include "Core/Backend/BackendManager.h"
#include "3rdParty/ImGui.h"
#include "Core/SlabCore.h"
#include "utils/enums.h"


#define Unique(label) \
    Str(Str(label) + "##" + ToStr(this->id)).c_str()


namespace Slab::Graphics {

    CountType Scene3DWindow::WindowCount = 0;

    Scene3DWindow::Scene3DWindow()
    : FSlabWindow(FSlabWindowConfig(Str("Scene3D (" + ToStr(WindowCount+1) +")")))
    , id(++WindowCount)
    {
        Core::LoadModule("ImGui");
        Core::LoadModule("ModernOpenGL");

        SetClear(false);
    }

    void Scene3DWindow::ImmediateDraw(const FPlatformWindow& PlatformWindow) {
        FSlabWindow::ImmediateDraw(PlatformWindow);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        // glDepthFunc(GL_GREATER);
        // glDepthFunc(GL_ALWAYS);

        updateCamera();

        for(auto &actor : content) {
            if(actor->isVisible()) actor->draw(*this);
        }

        drawGUI();
    }

    void Scene3DWindow::updateCamera() {
        glm::vec3 target = camera.target;
        glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

        fix r = cameraDist;
        fix θ = cameraAnglePolar;
        fix φ = cameraAngleAzimuth;
        fix x = target.x + r*sin(θ)*cos(φ);
        fix y = target.y + r*sin(θ)*sin(φ);
        fix z = target.z     + r*cos(θ);

        camera.pos = {x, y, z};
        camera.aspect = (float) GetWidth() / (float) GetHeight();
    }

    bool Scene3DWindow::NotifyMouseMotion(int x, int y, int dx, int dy) {
        FSlabWindow::NotifyMouseMotion(x, y, dx, dy);

        fix Mouse = GetMouseState();

        if(Mouse->IsLeftPressed() && Mouse->IsRightPressed()) {
            fix scaled_dx = -dx * 1.e-2f;
            fix scaled_dy =  dy * 1.e-2f;

            auto fwd3d = camera.target - camera.pos;

            auto fwd = glm::normalize(glm::vec3(fwd3d.x, fwd3d.y, .0f));
            auto side = glm::cross(fwd, camera.up);

            camera.target += fwd*scaled_dy + side*scaled_dx;

            return true;
        }

        if(Mouse->IsLeftPressed()) {
            cameraAngleAzimuth -= .0025f * dx;
            cameraAnglePolar   -= .0025f * dy;

            cameraAnglePolar = std::min(std::max(cameraAnglePolar, 1.e-5f), (1.f-1.e-5f)*static_cast<float>(M_PI));

            return true;
        }

        if(Mouse->IsRightPressed()) {
            cameraDist += dy*.01f;

            return true;
        }

        if(Mouse->IsCenterPressed()) {
            camera.yFov += dy*.005f;

            return true;
        }

        return false;
    }

    bool Scene3DWindow::NotifyMouseWheel(double dx, double dy) {
        camera.yFov += .01f*camera.yFov*(float)dy;

        return FSlabWindow::NotifyMouseWheel(dx, dy);
    }

    auto Scene3DWindow::getCamera() const -> const Camera & {
        return camera;
    }

    bool Scene3DWindow::addActor(const Actor_ptr& actor) {
        if(Contains(content, actor)) return false;

        content.emplace_back(actor);

        return true;
    }

    void Scene3DWindow::drawGUI() {
        // auto popupName = title + Str(" window popup");

        if (showInterface) {
            auto vp = GetViewport();
            // auto sh = Slab::Graphics::GetGraphicsBackend()->getScreenHeight();
            // ImGui::SetNextWindowPos({(float)vp.xMin, (float)(sh-(vp.yMin+vp.height()))}, ImGuiCond_Appearing);
            ImGui::SetNextWindowPos({(float)vp.xMin, (float)vp.yMin}, ImGuiCond_Appearing);
            ImGui::SetNextWindowSize({(float)vp.GetWidth()*.20f, (float)vp.GetHeight()}, ImGuiCond_Appearing);

            if (ImGui::Begin(title.c_str(), &showInterface)) {

                for (IN artie: content) {
                    bool visible = artie->isVisible();

                    if (ImGui::Checkbox(Unique(artie->getLabel()), &visible))
                        artie->setVisibility(visible);
                }

                for (IN artie: content) {
                    if (artie->isVisible() && artie->hasGUI()) {
                        if (ImGui::CollapsingHeader(Unique(artie->getLabel())))
                            artie->drawGUI();
                    }
                }
            }

            ImGui::End();
        }
    }

    bool Scene3DWindow::NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) {
        if(key == EKeyMap::Key_TAB && state == EKeyState::Release && modKeys.Mod_Shift == Press) {
            showInterface = !showInterface;
            return true;
        }

        return FSlabWindow::NotifyKeyboard(key, state, modKeys);
    }


} // Graphics