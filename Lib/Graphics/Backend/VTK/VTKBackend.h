//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_VTKBACKEND_H
#define STUDIOSLAB_VTKBACKEND_H

#include "Graphics/Backend/GraphicBackend.h"

namespace Slab::Graphics {

    class FVTKBackend : public FGraphicBackend {
    public:
        void Run() override;
    };

    using VTKBackend [[deprecated("Use FVTKBackend")]] = FVTKBackend;

}


#endif //STUDIOSLAB_VTKBACKEND_H
