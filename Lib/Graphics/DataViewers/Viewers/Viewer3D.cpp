//
// Created by joao on 21/12/24.
//

#include "Viewer3D.h"

namespace Slab::Graphics {

    FViewer3D::FViewer3D(TPointer<FGUIWindow> gui_window) : FViewer(gui_window) {
        scene3DWindow = New<FScene3DWindow>();

        AddWindow(scene3DWindow);
    }

    Str FViewer3D::GetName() const { return "3D viewer"; }

    void FViewer3D::SetFunction(TPointer<Math::R2toR::FNumericFunction> function) {
        if(getFunction() != nullptr) throw Exception("FViewer3D already set-up with function.");

        FViewer::SetFunction(function);

        sceneActor = New<FR2toRFunctionActor>(function);
        scene3DWindow->addActor(sceneActor);
    }

} // Graphics::Slab