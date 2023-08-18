//
// Created by joao on 4/8/23.
//

#include "FlatFieldDisplay.h"
#include "Mappings/R2toR/Model/R2toRDiscreteFunction.h"

#include <utility>



R2toR::Graphics::FlatFieldDisplay::FlatFieldDisplay(R2toR::Function::ConstPtr function, Resolution w, Resolution h)
: func(std::move(function)), texture((int)w, (int)h)
{
    if(!func->isDiscrete())
        throw "R2toR::Graphics::FlatFieldDisplay don't know what to do with not-discrete function";
}

void R2toR::Graphics::FlatFieldDisplay::draw() {
    assert(func->isDiscrete());

    Window::draw();

    auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);


}

