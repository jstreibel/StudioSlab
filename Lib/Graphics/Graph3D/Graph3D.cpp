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
    : cameraPos({0, 1, 0.5f}) {
        Core::BackendManager::LoadModule(Core::ImGui);
        Core::BackendManager::LoadModule(Core::ModernOpenGL);

        actors.emplace_back(std::make_shared<Field2DActor>());

        updateProjectionMatrix();

    }

    void Graph3D::draw() {
        Window::draw();

        ImGui::Begin("Graph3D debug");
        ImGui::Text("pitch %f",   pitch);
        ImGui::Text("yaw %f",     yaw);
        auto cam = cameraPos;
        ImGui::Text("Camera @ [%f, %f, %f]", cam[0], cam[1], cam[2]);
        ImGui::End();

        for(auto &actor : actors) actor->draw(*this);
    }

    auto Graph3D::getProjection() const -> glm::mat4 {
        return projection;
    }
    auto Graph3D::getViewTransform()  const -> glm::mat4 {
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

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

            float fov = M_PI * 60.0f / 180.0f;

            float nearPlane = 0.1f;
            float farPlane = 100.0f;

            projection = glm::perspective(fov, aspect, nearPlane, farPlane);

        } else {
            fix L = 1.f;
            fix yMin = -.5f*L;
            fix yMax = +.5f*L;
            fix xMin = yMin*aspect;
            fix xMax = yMax*aspect;
            fix zMin = -.5f*L;
            fix zMax = +.5f*L;
            // projection = glm::ortho(xMin, xMax, yMin, yMax, -20.f, 20.f);
        }
    }

    bool Graph3D::notifyMouseMotion(int x, int y) {
        if(Window::isMouseLeftClicked()) {
            auto mouseState = Core::BackendManager::GetGUIBackend().getMouseState();

            fix dx = (float)mouseState.dx;
            fix dy = (float)mouseState.dy;

            fix zxAngle = dy*.25f;
            fix yzAngle = dx*.25f;

            yaw += yzAngle;
            pitch += zxAngle;

            return true;
        }
        else if(Window::isMouseRightClicked()) {
            auto mouseState = Core::BackendManager::GetGUIBackend().getMouseState();

            fix dx = (float)mouseState.dx*.01f;
            fix dy = (float)mouseState.dy*.01f;

            cameraPos[0] += dx;
            cameraPos[2] += dy;

            return true;
        }

        return GUIEventListener::notifyMouseMotion(x, y);
    }

    void Graph3D::notifyReshape(int w, int h) {
        Window::notifyReshape(w, h);

        updateProjectionMatrix();
    }


} // Graphics