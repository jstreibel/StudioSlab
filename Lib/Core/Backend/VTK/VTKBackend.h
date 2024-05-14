//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_VTKBACKEND_H
#define STUDIOSLAB_VTKBACKEND_H

#include "Core/Backend/GraphicBackend.h"

class VTKBackend : public GraphicBackend {
public:
    void run(Program *integrator) override;
};


#endif //STUDIOSLAB_VTKBACKEND_H
