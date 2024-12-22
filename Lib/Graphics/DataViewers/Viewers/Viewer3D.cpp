//
// Created by joao on 21/12/24.
//

#include "Viewer3D.h"

namespace Slab::Graphics {

    Viewer3D::Viewer3D(Pointer<GUIWindow> gui_window) : Viewer(gui_window) {
        scene3DWindow = New<Scene3DWindow>();

        addWindow(scene3DWindow);
    }

    Str Viewer3D::getName() const { return "3D viewer"; }

    void Viewer3D::setFunction(Pointer<Math::R2toR::NumericFunction> function) {
        if(getFunction() != nullptr) throw Exception("Viewer3D already set-up with function.");

        Viewer::setFunction(function);

        sceneActor = New<R2toRFunctionActor>(function);
        scene3DWindow->addActor(sceneActor);
    }

} // Graphics::Slab