//
// Created by joao on 4/10/23.
//

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "Graph3D.h"
#include "Field2DActor.h"
#include "Core/Backend/BackendManager.h"
#include "imgui.h"


namespace Graphics {

    Graph3D::Graph3D()
    {
        Core::BackendManager::LoadModule(Core::ImGui);
        Core::BackendManager::LoadModule(Core::ModernOpenGL);

        actors.emplace_back(std::make_shared<Field2DActor>());
    }

    void Graph3D::draw() {
        Window::draw();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        updateCamera();

        for(auto &actor : actors) actor->draw(*this);
    }

    void Graph3D::updateCamera() {
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

    bool Graph3D::notifyMouseMotion(int x, int y) {
        if(Window::isMouseLeftClicked()) {
            auto mouseState = Core::BackendManager::GetGUIBackend().getMouseState();

            fix dx = (float)mouseState.dx;
            fix dy = (float)mouseState.dy;

            cameraAngleAzimuth -= dx*.0025f;
            cameraAnglePolar   += dy*.0025f;

            return true;
        }
        else if(Window::isMouseRightClicked()) {
            auto mouseState = Core::BackendManager::GetGUIBackend().getMouseState();

            fix dx = (float)mouseState.dx;
            fix dy = (float)mouseState.dy;

            cameraDist += dy*.01f;

            return true;
        }

        return GUIEventListener::notifyMouseMotion(x, y);
    }

    bool Graph3D::notifyMouseWheel(double dx, double dy) {
        camera.yFov += .01f*camera.yFov*(float)dy;

        return GUIEventListener::notifyMouseWheel(dx, dy);
    }

    auto Graph3D::getCamera() const -> const Camera & {
        return camera;
    }


} // Graphics