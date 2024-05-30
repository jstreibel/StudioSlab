//
// Created by joao on 4/10/23.
//

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "Scene3DWindow.h"
#include "Graphics/Graph3D/Actors/R2toRFunctionActor.h"
#include "Core/Backend/BackendManager.h"
#include "imgui.h"


#define Unique(label) \
    Str(Str(label) + "##" + ToStr(this->id)).c_str()


namespace Slab::Graphics {

    Count Scene3DWindow::WindowCount = 0;

    Scene3DWindow::Scene3DWindow() : id(++WindowCount)
    {
        Core::BackendManager::LoadModule(Core::ImGui);
        Core::BackendManager::LoadModule(Core::ModernOpenGL);
    }

    void Scene3DWindow::draw() {
        Window::draw();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        updateCamera();

        for(auto &actor : content) {
            if(actor->isVisible()) actor->draw(*this);
        }

        drawGUI();
    }

    void Scene3DWindow::updateCamera() {
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

        fix r = cameraDist;
        fix θ = cameraAnglePolar;
        fix φ = cameraAngleAzimuth;
        fix x = r*sin(θ)*cos(φ);
        fix y = r*sin(θ)*sin(φ);
        fix z = r*cos(θ);

        camera.pos = {x, y, z};
        camera.aspect = (float)getw()/(float)geth();
    }

    bool Scene3DWindow::notifyMouseMotion(int x, int y) {
        if(Window::isMouseLeftClicked()) {
            auto mouseState = Core::BackendManager::GetGUIBackend().getMouseState();

            fix dx = (float)mouseState.dx;
            fix dy = (float)mouseState.dy;

            cameraAngleAzimuth -= dx*.0025f;
            cameraAnglePolar   -= dy*.0025f;

            cameraAnglePolar = std::min(std::max(cameraAnglePolar, 1.e-5f), (1.f-1.e-5f)*(float)M_PI);

            return true;
        }
        else if(Window::isMouseRightClicked()) {
            auto mouseState = Core::BackendManager::GetGUIBackend().getMouseState();

            fix dy = (float)mouseState.dy;

            cameraDist += dy*.01f;

            return true;
        }
        else if(Window::isMouseCenterClicked()) {
            auto mouseState = Core::BackendManager::GetGUIBackend().getMouseState();

            fix dy = (float)mouseState.dy;

            camera.yFov += dy*.005f;

            return true;
        }

        return GUIEventListener::notifyMouseMotion(x, y);
    }

    bool Scene3DWindow::notifyMouseWheel(double dx, double dy) {
        camera.yFov += .01f*camera.yFov*(float)dy;

        return GUIEventListener::notifyMouseWheel(dx, dy);
    }

    auto Scene3DWindow::getCamera() const -> const Camera & {
        return camera;
    }

    bool Scene3DWindow::addActor(const Actor_ptr& actor) {
        if(Common::Contains(content, actor)) return false;

        content.emplace_back(actor);

        return true;
    }

    void Scene3DWindow::drawGUI() {
        // auto popupName = title + Str(" window popup");

        if (showInterface) {
            auto vp = getViewport();
            auto sh = Core::BackendManager::GetGUIBackend().getScreenHeight();

            ImGui::SetNextWindowPos({(float)vp.xMin, (float)(sh-(vp.yMin+vp.height()))}, ImGuiCond_Appearing);
            ImGui::SetNextWindowSize({(float)vp.width()*.20f, (float)vp.height()}, ImGuiCond_Appearing);

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

    bool Scene3DWindow::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        if(key == Key_TAB && state == Release) {
            showInterface = !showInterface;
            return true;
        }

        return GUIEventListener::notifyKeyboard(key, state, modKeys);
    }


} // Graphics