//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_VTKBACKEND_H
#define STUDIOSLAB_VTKBACKEND_H

#include "Graphics/Backend/GraphicBackend.h"

namespace Slab::Core {

    class VTKBackend : public GraphicBackend {
    public:
        void run() override;
    };

}


#endif //STUDIOSLAB_VTKBACKEND_H
