//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_R2TORMODELBUILDER_H
#define FIELDS_R2TORMODELBUILDER_H


#include "Core/Allocator/Allocator.h"


class R2toRModelBuilder : public Allocator {
public:
    R2toRModelBuilder(NumericParams &p, Device &dev, po::variables_map &vm);

    auto newFunctionArbitrary() -> void * override;

    auto newFieldState() -> void * override;

    auto getSystemSolver() -> void * override;
};


#endif //FIELDS_R2TORMODELBUILDER_H
