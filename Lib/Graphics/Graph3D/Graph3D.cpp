//
// Created by joao on 4/10/23.
//

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "Graph3D.h"
#include "Field2DActor.h"
#include "Core/Backend/BackendManager.h"


namespace Graphics {

    Graph3D::Graph3D() {
        actors.emplace_back(std::make_shared<Field2DActor>());

        updateProjectionMatrix();
    }

    void Graph3D::draw() {
        Window::draw();

        for(auto &actor : actors) actor->draw(*this);
    }

    auto Graph3D::getProjection() const -> glm::mat4 { return projection; }
    auto Graph3D::getViewTransform()  const -> glm::mat4 {
        glm::mat4 view = glm::mat4(1.0f);

        // Create rotation matrices
        glm::mat4 pitchMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));

        // Combine rotations
        view = yawMatrix * pitchMatrix;

        return view;
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

            fix dx = (float)mouseState.dx;
            fix dy = (float)mouseState.dy*0.1f;

            // view = glm::translate(view, glm::vec3(0,0,dy));

            return true;
        }

        return GUIEventListener::notifyMouseMotion(x, y);
    }

    void Graph3D::updateProjectionMatrix() {
        float aspect = (float)getw()/(float)geth();   // Aspect ratio

        if(false) {
            float fov = glm::radians(45.0f);  // FOV 45 degrees
            float nearPlane = 0.1f;           // Near clipping plane
            float farPlane = 100.0f;          // Far clipping plane

            projection = glm::perspective(fov, aspect, nearPlane, farPlane);
        } else {
            fix yMin = -1.f;
            fix yMax =  1.f;
            fix xMin = yMin*aspect;
            fix xMax = yMax*aspect;
            projection = glm::ortho(xMin, xMax, yMin, yMax, -1.f, 1.f);
        }
    }

    void Graph3D::notifyReshape(int w, int h) {
        Window::notifyReshape(w, h);

        updateProjectionMatrix();
    }


} // Graphics