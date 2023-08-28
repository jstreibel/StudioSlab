//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_VTKBACKEND_H
#define STUDIOSLAB_VTKBACKEND_H

#include "Core/Backend/Backend.h"

class VTKBackend : public Backend {
public:
    void run(Program *integrator) override;
};


#endif //STUDIOSLAB_VTKBACKEND_H
