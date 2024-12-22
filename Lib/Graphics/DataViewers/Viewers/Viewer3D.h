//
// Created by joao on 21/12/24.
//

#ifndef STUDIOSLAB_VIEWER3D_H
#define STUDIOSLAB_VIEWER3D_H

#include "Viewer.h"
#include "Graphics/Plot3D/Scene3DWindow.h"
#include "Graphics/Plot3D/Actors/R2toRFunctionActor.h"

namespace Slab::Graphics {

    class Viewer3D : public Viewer {
        Pointer<Scene3DWindow> scene3DWindow;
        Pointer<R2toRFunctionActor> sceneActor;
    public:
        Viewer3D(Pointer<GUIWindow>);

        void setFunction(Pointer<Math::R2toR::NumericFunction> function) override;

        Str getName() const override;
    };

} // Graphics::Slab

#endif //STUDIOSLAB_VIEWER3D_H
