//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_SCENE3DWINDOW_H
#define STUDIOSLAB_SCENE3DWINDOW_H

#include "Graphics/Window/Window.h"
#include "Actor.h"
#include "Camera.h"

#include <glm/detail/type_mat4x4.hpp>

#include <list>

namespace Slab::Graphics {

    class Scene3DWindow : public Window {
        static Count WindowCount;
        Count id;

        Str title = "No title";

        Camera camera;
        float cameraDist = 9.f;
        float cameraAnglePolar = .25*M_PI;
        float cameraAngleAzimuth = .0f;

        bool showInterface = false;

        std::list<Actor_ptr> content;

        void updateCamera();
        void drawGUI();
    public:
        Scene3DWindow();

        void draw() override;
        auto getCamera() const -> const Camera&;

        bool notifyMouseMotion(int x, int y) override;
        bool notifyMouseWheel(double dx, double dy) override;

        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

        bool addActor(const Actor_ptr& actor);
    };

    DefinePointers(Scene3DWindow)

} // Graphics

#endif //STUDIOSLAB_SCENE3DWINDOW_H
