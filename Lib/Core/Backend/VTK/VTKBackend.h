//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_VTKBACKEND_H
#define STUDIOSLAB_VTKBACKEND_H

#include "Core/Backend/GUIBackend.h"

class VTKBackend : public GUIBackend {
public:
    void run(Program *integrator) override;
};


#endif //STUDIOSLAB_VTKBACKEND_H
