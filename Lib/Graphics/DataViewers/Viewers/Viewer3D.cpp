//
// Created by joao on 21/12/24.
//

#include "Viewer3D.h"

namespace Slab::Graphics {

    Viewer3D::Viewer3D(TPointer<FGUIWindow> gui_window) : Viewer(gui_window) {
        scene3DWindow = New<Scene3DWindow>();

        AddWindow(scene3DWindow);
    }

    Str Viewer3D::GetName() const { return "3D viewer"; }

    void Viewer3D::SetFunction(TPointer<Math::R2toR::FNumericFunction> function) {
        if(getFunction() != nullptr) throw Exception("Viewer3D already set-up with function.");

        Viewer::SetFunction(function);

        sceneActor = New<R2toRFunctionActor>(function);
        scene3DWindow->addActor(sceneActor);
    }

} // Graphics::Slab