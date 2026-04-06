//
// Created by joao on 21/12/24.
//

#ifndef STUDIOSLAB_VIEWER3D_H
#define STUDIOSLAB_VIEWER3D_H

#include "Viewer.h"
#include "Graphics/Plot3D/Scene3DWindow.h"
#include "Graphics/Plot3D/Actors/R2toRFunctionActor.h"

namespace Slab::Graphics {

    class FViewer3D : public FViewer {
        TPointer<FScene3DWindow> scene3DWindow;
        TPointer<FR2toRFunctionActor> sceneActor;
    public:
        FViewer3D(TPointer<FGUIWindow>);

        void SetFunction(TPointer<Math::R2toR::FNumericFunction> function) override;

        Str GetName() const override;
    };

    using Viewer3D [[deprecated("Use FViewer3D")]] = FViewer3D;

} // Graphics::Slab

#endif //STUDIOSLAB_VIEWER3D_H
