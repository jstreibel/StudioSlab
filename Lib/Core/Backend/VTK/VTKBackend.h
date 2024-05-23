//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_VTKBACKEND_H
#define STUDIOSLAB_VTKBACKEND_H

#include "Core/Backend/GraphicBackend.h"

namespace Slab::Core {

    class VTKBackend : public GraphicBackend {
    public:
        void run(Task *integrator) override;
    };

}


#endif //STUDIOSLAB_VTKBACKEND_H
