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

        updateProjectionMatrix();

    }

    void Graph3D::draw() {
        Window::draw();

        fix r = cameraDist_xy;
        fix θ = cameraAngle;
        fix x = r*cos(θ);
        fix y = r*sin(θ);
        fix z = cameraHeight;

        ImGui::Begin("Graph3D debug");
        ImGui::Text("Camera rθz: [%f, %f, %f]", r, θ, z);
        ImGui::Text("Camera xyz: [%f, %f, %f]", x, y, z);
        ImGui::End();

        for(auto &actor : actors) actor->draw(*this);
    }

    auto Graph3D::getProjection() const -> glm::mat4 {
        return projection;
    }
    auto Graph3D::getViewTransform()  const -> glm::mat4 {
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

        fix r = cameraDist_xy;
        fix θ = cameraAngle;
        fix x = r*cos(θ);
        fix y = r*sin(θ);
        fix z = cameraHeight;

        glm::vec3 cameraPos({x, y, z});

        glm::mat4 view = glm::lookAt(cameraPos, target, up);

        return view;
    }

    void Graph3D::updateProjectionMatrix() {
        float aspect = (float)getw()/(float)geth();   // Aspect ratio

        if(true) {
            // float fovY = 2.0f * glm::atan(d / (2.0f * camHeight));
            // float fovX = 2.0f * glm::atan(w / (2.0f * camHeight));
            // float fov = glm::max(fovX, fovY);
            // float aspect = w / d;  // or use the aspect ratio of your viewport

            float nearPlane = 0.1f;
            float farPlane = 100.0f;

            projection = glm::perspective(fovY, aspect, nearPlane, farPlane);

        }
    }

    bool Graph3D::notifyMouseMotion(int x, int y) {
        if(Window::isMouseLeftClicked()) {
            auto mouseState = Core::BackendManager::GetGUIBackend().getMouseState();

            fix dx = (float)mouseState.dx;
            fix dy = (float)mouseState.dy;

            cameraAngle += dx*.0025f;
            cameraDist_xy += dy*.0025f;

            return true;
        }
        else if(Window::isMouseRightClicked()) {
            auto mouseState = Core::BackendManager::GetGUIBackend().getMouseState();

            fix dx = (float)mouseState.dx;
            fix dy = (float)mouseState.dy;

            cameraHeight += dy*.0025f;

            return true;
        }

        return GUIEventListener::notifyMouseMotion(x, y);
    }

    void Graph3D::notifyReshape(int w, int h) {
        Window::notifyReshape(w, h);

        updateProjectionMatrix();
    }

    bool Graph3D::notifyMouseWheel(double dx, double dy) {
        fovY += .01f*fovY*(float)dy;

        updateProjectionMatrix();

        return GUIEventListener::notifyMouseWheel(dx, dy);
    }


} // Graphics