//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_SCENE3DWINDOW_H
#define STUDIOSLAB_SCENE3DWINDOW_H

#include "Graphics/Window/SlabWindow.h"
#include "Actor.h"
#include "Camera.h"

#include <glm/detail/type_mat4x4.hpp>

#include <list>

namespace Slab::Graphics {

    class FScene3DWindow : public FSlabWindow {
        static CountType WindowCount;
        CountType id;

        Str title = "No title";

        Camera camera;
        float cameraDist = 9.f;
        float cameraAnglePolar = .25*M_PI;
        float cameraAngleAzimuth = .0f;

        bool showInterface = false;

        std::list<FActor_ptr> content;

        void updateCamera();
        void drawGUI();
    public:
        FScene3DWindow();

        void ImmediateDraw(const FPlatformWindow&) override;
        auto getCamera() const -> const Camera&;

        bool NotifyMouseMotion(int x, int y, int dx, int dy) override;
        bool NotifyMouseWheel(double dx, double dy) override;

        bool NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) override;

        bool addActor(const FActor_ptr& actor);
    };

    DefinePointers(FScene3DWindow)

    using Scene3DWindow [[deprecated("Use FScene3DWindow")]] = FScene3DWindow;
    using Scene3DWindow_ptr [[deprecated("Use FScene3DWindow_ptr")]] = FScene3DWindow_ptr;
    using Scene3DWindow_constptr [[deprecated("Use FScene3DWindow_constptr")]] = FScene3DWindow_constptr;
    using Scene3DWindow_ref [[deprecated("Use FScene3DWindow_ref")]] = FScene3DWindow_ref;
    using Scene3DWindow_constref [[deprecated("Use FScene3DWindow_constref")]] = FScene3DWindow_constref;

} // Graphics

#endif //STUDIOSLAB_SCENE3DWINDOW_H
