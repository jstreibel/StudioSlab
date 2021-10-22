#ifndef FIELDBUILDER_H
#define FIELDBUILDER_H

#include "Core/NumericParams.h"
#include "Core/Device.h"

#include "Model/System.h"

class Allocator
{
    static Allocator* mySingleInstance;

protected:

    Allocator();
    static void Instantiate(Allocator *builder);

public:

    static auto getInstance() -> Allocator &;

    virtual auto newFunctionArbitrary() -> void * = 0;
	virtual auto newFieldState() -> void * = 0;
    virtual auto getSystemSolver() -> void * = 0;

    auto getNumericParams() const -> const NumericParams &;
	auto getDevice() const -> const Device &;


protected:
	NumericParams numericParams;
	Device dev;

};

#endif // FIELDBUILDER_H
