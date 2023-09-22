//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_IMGUIMODULE_H
#define STUDIOSLAB_IMGUIMODULE_H

#include "../Implementations.h"
#include "Module.h"

namespace Core {

    class ImGuiModule : public Module {
        static void GeneralInitialization();

        static void InstallInGLUT();
        static void InstallInGLFW();
        static void InstallInSFML();

        static void BuildFonts();
    public:
        explicit ImGuiModule(BackendImplementation);
        ~ImGuiModule() override;

        void beginRender() override;

        void endRender() override;

        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        bool notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) override;

        bool notifyMouseMotion(int x, int y) override;

        bool notifyMouseWheel(double dx, double dy) override;

        bool notifyFilesDropped(StrVector paths) override;

        bool notifyScreenReshape(int newScreenWidth, int newScreenHeight) override;

        bool notifyRender() override;
    };

} // Core

#endif //STUDIOSLAB_IMGUIMODULE_H
